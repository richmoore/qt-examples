#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QStringList>

#include "sslserver.h"

struct SslServerPrivate
{
    QSslCertificate cert;
    QSslKey key;
    QSslSocket *sock;
};

SslServer::SslServer(const QSslCertificate &cert, const QSslKey &key, int port)
    : QTcpServer(),
      d(new SslServerPrivate)
{
    d->cert = cert;
    d->key = key;
    d->sock = 0;

    listen(QHostAddress::Any, port);
}

SslServer::~SslServer()
{
    delete d;
}

void SslServer::errors(const QList<QSslError> &errs)
{
    qDebug() << Q_FUNC_INFO;

    foreach (const QSslError &err, errs) {
#if QT_VERSION >= 0x050000
        QStringList names = err.certificate().subjectInfo(QSslCertificate::CommonName);
        qDebug() << err << names;
#else
    qDebug() << err << err.certificate().subjectInfo(QSslCertificate::CommonName);
#endif
    }
}

void SslServer::ready()
{
    qDebug() << Q_FUNC_INFO;

    d->sock->write("This message is encrypted\n");

    // Slightly evil, but since want to close immediately in this example
    // we aren't going to wait for the main event loop.
    d->sock->waitForBytesWritten(1000);
    d->sock->close();
}

#if QT_VERSION >= 0x050000
void SslServer::incomingConnection(qintptr socketDescriptor)
#else
void SslServer::incomingConnection(int socketDescriptor)
#endif
{
    qDebug() << Q_FUNC_INFO;

    d->sock = new QSslSocket(this);
    if (!d->sock->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Unable set socket descriptor";

        delete d->sock;
        d->sock = 0;
        return;
    }

    connect(d->sock, SIGNAL(sslErrors(const QList<QSslError> &)),
            this, SLOT(errors(const QList<QSslError> &)));
    connect(d->sock, SIGNAL(encrypted()), this, SLOT(ready()));

    d->sock->setLocalCertificate(d->cert);
    d->sock->setPrivateKey(d->key);

    d->sock->startServerEncryption();
}

