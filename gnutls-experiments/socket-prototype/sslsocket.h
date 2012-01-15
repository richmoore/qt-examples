#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QObject>
#include <gnutls/gnutls.h>

struct SslSocketPrivate;

class SslSocket : public QObject
{
    Q_OBJECT

public:
    SslSocket(QObject *parent=0);
    ~SslSocket();

    QByteArray read(qint64 maxsize);
    
public slots:
    void connectToHost(const QString &hostname, int port);
    void startHandshake();

    qint64 write(const QByteArray &data);

signals:
    void handshakeComplete();
    void readyRead();
    void error();

private slots:
    void dataReceived();

private:
    void setupSession();
    void handshakeInternal();

    ssize_t readInternal(void *buffer, size_t length);
    ssize_t writeInternal(const void *buffer, size_t length);
    ssize_t writeVectorInternal(const giovec_t *iov, int iovcnt);

    static ssize_t read_callback(gnutls_transport_ptr_t transport, void *buffer, size_t length);
    static ssize_t write_callback(gnutls_transport_ptr_t transport, const void *buffer, size_t length);
    static ssize_t write_vector_callback(gnutls_transport_ptr_t transport, const giovec_t *iov, int iovcnt);

    SslSocketPrivate *d;
};

#endif // SSLSOCKET_H

