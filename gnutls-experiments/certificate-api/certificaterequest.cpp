#include <QByteArray>
#include <QIODevice>

#include "certificaterequest_p.h"

static QByteArray request_to_bytearray(gnutls_x509_crq_t crq, gnutls_x509_crt_fmt_t format, int *errno)
{
    QByteArray ba(4096, 0);
    size_t size = ba.size();

    *errno = gnutls_x509_crq_export(crq, format, ba.data(), &size);

    if (GNUTLS_E_SUCCESS != *errno)
        return QByteArray();

    ba.resize(size); // size has now been updated
    return ba;
}

CertificateRequestPrivate::CertificateRequestPrivate()
    : null(true)
{
    gnutls_x509_crq_init(&crq);
    errno = GNUTLS_E_SUCCESS;
}

CertificateRequestPrivate::~CertificateRequestPrivate()
{
    gnutls_x509_crq_deinit(crq);
}

CertificateRequest::CertificateRequest()
    : d(new CertificateRequestPrivate)
{
}

CertificateRequest::CertificateRequest(const CertificateRequest &other)
    : d(other.d)
{
}

CertificateRequest::CertificateRequest(QIODevice *io, QSsl::EncodingFormat format)
    : d(new CertificateRequestPrivate)
{
    QByteArray buf = io->readAll();

    // Setup a datum
    gnutls_datum_t buffer;
    buffer.data = (unsigned char *)(buf.data());
    buffer.size = buf.size();

    d->errno = gnutls_x509_crq_import(d->crq, &buffer, (QSsl::Pem == format) ? GNUTLS_X509_FMT_PEM : GNUTLS_X509_FMT_DER);
    if (GNUTLS_E_SUCCESS == d->errno)
        d->null = false;
}

CertificateRequest::~CertificateRequest()
{
}

bool CertificateRequest::isNull() const
{
    return d->null;
}

int CertificateRequest::error() const
{
    return d->errno;
}

QString CertificateRequest::errorString() const
{
    return QString::fromUtf8(gnutls_strerror(d->errno));
}

QByteArray CertificateRequest::toPem()
{
    return request_to_bytearray(d->crq, GNUTLS_X509_FMT_PEM, &d->errno);
}

QByteArray CertificateRequest::toDer()
{
    return request_to_bytearray(d->crq, GNUTLS_X509_FMT_DER, &d->errno);
}
