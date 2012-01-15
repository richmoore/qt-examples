#include <QTcpSocket>
#include <QDebug>
#include <QCoreApplication>

#include <gnutls/gnutls.h>

#include <errno.h>

#include "sslsocket.h"

enum State
{
    Disconnected,
    Connecting,
    Handshaking,
    Encrypted
};


struct SslSocketPrivate
{
    SslSocketPrivate() :
        state(Disconnected),
        socket(0),
        session(0)
    {
    }

    State state;
    QTcpSocket *socket;
    gnutls_session_t session;
    gnutls_certificate_credentials_t x509cred;
    gnutls_priority_t priority_cache;
};

SslSocket::SslSocket(QObject *parent)
    : QObject(parent),
      d(new SslSocketPrivate)
{
    d->socket = new QTcpSocket(this);
    setupSession();
}

SslSocket::~SslSocket()
{
    gnutls_priority_deinit(d->priority_cache);
    gnutls_deinit(d->session);
    gnutls_certificate_free_credentials(d->x509cred);
    delete d;
}

void SslSocket::setupSession()
{
    qDebug() << "Initialise client session";

    // Setup the trust store
    gnutls_certificate_allocate_credentials(&d->x509cred);
    gnutls_certificate_set_x509_trust_file(d->x509cred, "/etc/ssl/ca-bundle.pem", GNUTLS_X509_FMT_PEM);

    // Configure the session
    gnutls_init(&d->session, GNUTLS_CLIENT);
    gnutls_credentials_set(d->session, GNUTLS_CRD_CERTIFICATE, d->x509cred);

    const char *err;
    gnutls_priority_init(&d->priority_cache, "NORMAL", &err);
    gnutls_priority_set(d->session, d->priority_cache);

    // Setup the transport functions to use QTcpSocket
    gnutls_transport_set_ptr(d->session, this);
    gnutls_transport_set_pull_function(d->session, read_callback);

#ifdef NO_VECTOR_WRITES
    gnutls_transport_set_push_function(d->session, write_callback);
#else
    gnutls_transport_set_vec_push_function(d->session, write_vector_callback);
#endif
}

void SslSocket::connectToHost(const QString &hostname, int port)
{
    qDebug() << "connect to host" << hostname;
    connect(d->socket, SIGNAL(connected()), SLOT(startHandshake()));
    connect(d->socket, SIGNAL(readyRead()), SLOT(dataReceived()));

    d->socket->connectToHost(hostname, port);
    d->state = Connecting;
}
 
void SslSocket::startHandshake()
{
    d->state = Handshaking;
    handshakeInternal();
}

QByteArray SslSocket::read(qint64 maxsize)
{
    QByteArray buffer;

    if (d->state != Encrypted) {
        qFatal("Write before encrypted is not supported yet");
        return buffer;
    }

    buffer.resize(maxsize); // ### This could get pretty big!
    ssize_t result;
    do {
        result = gnutls_record_recv(d->session, buffer.data(), maxsize);
    } while( result == GNUTLS_E_INTERRUPTED || result == GNUTLS_E_AGAIN );

    buffer.truncate(result);

    return buffer;
}

qint64 SslSocket::write(const QByteArray &data)
{
    qDebug() << "write";

    if (d->state != Encrypted) {
        qFatal("Write before encrypted is not supported yet");
        return -1;
    }

    ssize_t result;
    do {
        // ### It's possible that this could fail due to the need to read some data
        // for example if a renegotiation is underway. Not sure.
        result = gnutls_record_send(d->session, data.constData(), data.size());
        qDebug() << "write result" << result;
    } while( result == GNUTLS_E_INTERRUPTED || result == GNUTLS_E_AGAIN );

    return result;
}

void SslSocket::dataReceived()
{
    qDebug() << "dataReceived() state is " << d->state;

    if (d->state == Handshaking) {
        handshakeInternal();
    }
    else if (d->state == Encrypted) {
        emit readyRead();
    }
}

void SslSocket::handshakeInternal()
{
    qDebug() << "start handshake, state is" << d->state;
    int result = gnutls_handshake(d->session);
    qDebug() << "shake result" << gnutls_strerror(result) << result;

    if (result == 0) {
        qDebug() << "handshake completed";
        d->state = Encrypted;
        emit handshakeComplete();
    }
    else if (gnutls_error_is_fatal(result)) {
        qDebug() << "fatal error in handshake";
        emit error();
        d->socket->disconnect();
        d->state = Disconnected;
    }
}

ssize_t SslSocket::readInternal(void *buffer, size_t length)
{
    qDebug() << "readInternal, length" << length << ", available" << d->socket->bytesAvailable();

    if (d->socket->bytesAvailable() < qint64(length)) {
        gnutls_transport_set_errno(d->session, EAGAIN);
        return -1;
    }

    return d->socket->read(static_cast<char *>(buffer), length);
}

ssize_t SslSocket::writeInternal(const void *buffer, size_t length)
{
    qDebug() << "writeInternal, length" << length;
    return d->socket->write(static_cast<const char *>(buffer), length);
}

ssize_t SslSocket::writeVectorInternal(const giovec_t *iov, int iovcnt)
{
    qDebug() << "writeVectorInternal, buffers" << iovcnt;

    ssize_t written = 0;
    for (int i=0; i < iovcnt; i++) {
        int count = d->socket->write(static_cast<const char *>(iov[i].iov_base), iov[i].iov_len);
        if (count < 0)
            return -1;
        written += count;
    }

    return written;
}

//
// Static callbacks that forward to private internal read/write methods
//

ssize_t SslSocket::read_callback(gnutls_transport_ptr_t transport, void *buffer, size_t length)
{
    SslSocket *self = static_cast<SslSocket *>(transport);
    return self->readInternal(buffer, length);
}

ssize_t SslSocket::write_callback(gnutls_transport_ptr_t transport, const void *buffer, size_t length)
{
    SslSocket *self = static_cast<SslSocket *>(transport);
    return self->writeInternal(buffer, length);
}

ssize_t SslSocket::write_vector_callback(gnutls_transport_ptr_t transport, const giovec_t *iov, int iovcnt)
{
    SslSocket *self = static_cast<SslSocket *>(transport);
    return self->writeVectorInternal(iov, iovcnt);
}
