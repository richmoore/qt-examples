#include <QDebug>

#include "sslsocket.h"


#include "getslash.h"


GetSlash::GetSlash(SslSocket *sock)
    : QObject()
{
    this->sock = sock;
    connect(sock, SIGNAL(handshakeComplete()), this, SLOT(start()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(gotData()));
}

void GetSlash::start()
{
    sock->write(QByteArray("GET / HTTP/1.0\n\n"));
}

void GetSlash::gotData()
{
    QByteArray result = sock->read(2000);
    qDebug() << result;
}
