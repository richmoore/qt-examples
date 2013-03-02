#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>

class QSslCertificate;
class QSslKey;

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
    void incomingConnection(int socketDescriptor);

private:
    SslServerPrivate *d;
};

#endif // SSLSERVER_H


