#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>

class QSslCertificate;
class QSslKey;
class QSslError;

struct SslServerPrivate;

class SslServer : public QTcpServer
{
    Q_OBJECT

public:
    SslServer(const QSslCertificate &cert, const QSslKey &key, int port=4433);
    ~SslServer();

protected slots:
    void ready();

protected:
#if QT_VERSION >= 0x050000
    void incomingConnection(qintptr socketDescriptor);
#else
    void incomingConnection(int socketDescriptor);
#endif

private:
    SslServerPrivate *d;
};

#endif // SSLSERVER_H


