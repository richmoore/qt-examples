#include <QDebug>
#include <QHostAddress>
#include <QTcpSocket>
#include <QSslSocket>
#include "httprequesthandler.h"

static qint64 MAX_REQUEST_LINE_LENGTH = 2048;

HttpRequestHandler::HttpRequestHandler(QTcpSocket *sock, QObject *parent)
    : QObject(parent),
      state(StateReadingRequest),
      sock(sock)
{
    connect(sock, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

HttpRequestHandler::~HttpRequestHandler()
{
}

QByteArray HttpRequestHandler::path() const
{
    return mpath;
}

void HttpRequestHandler::dataReceived()
{
    if (state == StateReadingRequest) {
        if (sock->canReadLine()) {
            QByteArray request = sock->readLine();

            QList<QByteArray> requestSections = request.split(' ');
            
            // We only support GET
            if (requestSections[0].toUpper() != QByteArrayLiteral("GET")) {
                qDebug() << "Not a get";
                sendError(400);
                deleteLater();
                return;
            }

            mpath = requestSections[1];
            state = StateReadingHeaders;
            // Fall through to reading headers
            qDebug() << "Got request for" << mpath;
        }
        else if (sock->bytesAvailable() > MAX_REQUEST_LINE_LENGTH) {
            qDebug() << "Too long";
            sendError(400);
            deleteLater();
            return;
        }
    }

    if (state == StateReadingHeaders) {
        while (sock->canReadLine()) {
            QByteArray request = sock->readLine();

            // Is this the end?
            if (request == QByteArrayLiteral("\r\n") || request == QByteArrayLiteral("\n")) {
                processRequest();
                state = StateDone;
                return;
            }

            int colon = request.indexOf(':');
            if (colon == -1) {
                qDebug() << "Bad header" << request.toHex();
                sendError(400);
                deleteLater();
                return;
            }

            QByteArray header = request.left(colon);
            QByteArray value = request.mid(colon+1);

            qDebug() << header.toUpper() << value.trimmed();
            headers.insert(header.toUpper(), value.trimmed());
        }
    }

    if (state == StateDone) {
        if (sock->bytesAvailable())
            qWarning("Data received when done!");
    }
}

void HttpRequestHandler::processRequest()
{
    QString headers("HTTP/1.1 200 OK\n" \
                    "Content-Length: %1\n" \
                    "\n");

    QString body("<html><body>" \
                 "<h1>It works!</h1>"         \
                 "HTTP connection from %1"    \
                 "</body></html>");
    body = body.arg(sock->peerAddress().toString());

    sock->write(headers.arg(body.toUtf8().length()).toUtf8());
    sock->write(body.toUtf8());
    sock->flush(); // Needed before change 86186
    sock->close();
    sock->deleteLater();
}

void HttpRequestHandler::sendError(int code)
{
    QString headers("HTTP/1.1 %1 FAIL\n" \
                    "Content-Length: %2\n" \
                    "\n");

    QByteArray body("<html><body><h1>Error :-(</h1></body></html>");

    sock->write(headers.arg(code).arg(body.length()).toUtf8());
    sock->write(body);
    sock->flush(); // Needed before change 86186
    sock->close();
    sock->deleteLater();
}
