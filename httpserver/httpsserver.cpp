#include <QDebug>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslConfiguration>
#include <QTcpSocket>

#include "qsslserver.h"
#include "httprequesthandler.h"
#include "httpsserver.h"

HttpsServer::HttpsServer(quint16 port, QObject *parent)
{
    QFile certFile("leaf.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to load certificate";
        return;
    }

    QFile keyFile("leaf.key");
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to load key";
        return;
    }

    QSslCertificate cert(&certFile);
    QSslKey key(&keyFile, QSsl::Rsa);

    server = new QSslServer(this);
    connect(server, SIGNAL(newEncryptedConnection()), this, SLOT(handleConnection()));

    QSslConfiguration config = server->sslConfiguration();
    config.setLocalCertificate(cert);
    config.setPrivateKey(key);
    server->setSslConfiguration(config);

    server->listen(QHostAddress::Any, port);
}

HttpsServer::~HttpsServer()
{
}

void HttpsServer::handleConnection()
{
    while(server->hasPendingConnections()) {
        QTcpSocket *sock = server->nextPendingConnection();
        HttpRequestHandler *handler = new HttpRequestHandler(sock, this);
    }
}
