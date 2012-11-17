#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include <QString>
#include <QIODevice>

#include "key.h"
#include "certificaterequest.h"

struct KeyPrivate
{
    int errno;
    gnutls_x509_privkey_t key;
};

struct CertificateRequestPrivate
{
    int errno;
    gnutls_x509_crq_t crq;
};

//
// CertificateRequest
//

CertificateRequest::CertificateRequest()
    : d(new CertificateRequestPrivate)
{
    gnutls_x509_crq_init(&d->crq);
    d->errno = GNUTLS_E_SUCCESS;
}

CertificateRequest::CertificateRequest(QIODevice *io, RequestFormat format)
    : d(new CertificateRequestPrivate)
{
    gnutls_x509_crq_init(&d->crq);

    QByteArray buf = io->readAll();

    // Setup a datum
    gnutls_datum_t buffer;
    buffer.data = (unsigned char *)(buf.data());
    buffer.size = buf.size();

    d->errno = gnutls_x509_crq_import(d->crq, &buffer, (FormatPem == format) ? GNUTLS_X509_FMT_PEM : GNUTLS_X509_FMT_DER);
}

CertificateRequest::~CertificateRequest()
{
    gnutls_x509_crq_deinit(d->crq);
    delete d;
}

int CertificateRequest::error() const
{
    return d->errno;
}

QString CertificateRequest::errorString() const
{
    return QString::fromUtf8(gnutls_strerror(d->errno));
}

bool CertificateRequest::setVersion(int version)
{
    d->errno = gnutls_x509_crq_set_version(d->crq, version);
    return GNUTLS_E_SUCCESS == d->errno;
}

int CertificateRequest::version() const
{
    int ver = gnutls_x509_crq_get_version(d->crq);
    if (ver < 0)
        d->errno = ver;
    return ver;
}

bool CertificateRequest::setKey(const Key *key)
{
    d->errno = gnutls_x509_crq_set_key(d->crq, key->d->key);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateRequest::addNameEntry(EntryType type, const QByteArray &value)
{
    QByteArray oid;

    // TODO: More common name entry types

    switch(type) {
    case EntryCountryName:
        oid = QByteArray(GNUTLS_OID_X520_COUNTRY_NAME);
        break;
    case EntryOrganizationName:
        oid = QByteArray(GNUTLS_OID_X520_ORGANIZATION_NAME);
        break;
    case EntryOrganizationalUnitName:
        oid = QByteArray(GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME);
        break;
    case EntryCommonName:
        oid = QByteArray(GNUTLS_OID_X520_COMMON_NAME);
        break;
    default:
        qWarning("Unhandled name entry type %d", int(type));
        return false;
    }

    return addNameEntry(oid, value);
}

bool CertificateRequest::addNameEntry(const QByteArray &oid, const QByteArray &value, bool raw)
{
    d->errno = gnutls_x509_crq_set_dn_by_oid(d->crq, oid.constData(), raw,
                                             value.constData(), qstrlen(value.constData()));
    return GNUTLS_E_SUCCESS == d->errno;   
}

bool CertificateRequest::sign(const Key *key)
{
    d->errno = gnutls_x509_crq_sign2(d->crq, key->d->key, GNUTLS_DIG_SHA1, 0);
    return GNUTLS_E_SUCCESS == d->errno;
}

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

QByteArray CertificateRequest::toPEM()
{
    return request_to_bytearray(d->crq, GNUTLS_X509_FMT_PEM, &d->errno);
}

QByteArray CertificateRequest::toDER()
{
    return request_to_bytearray(d->crq, GNUTLS_X509_FMT_DER, &d->errno);
}

//
// Key
//

Key::Key()
    : d(new KeyPrivate)
{
    gnutls_x509_privkey_init(&d->key);
    d->errno = GNUTLS_E_SUCCESS;
}

Key::~Key()
{
    gnutls_x509_privkey_deinit(d->key);
    delete d;
}

bool Key::generate( KeyType type, KeyStrength strength )
{
    // TODO: type, strength
    uint bits = gnutls_sec_param_to_pk_bits(GNUTLS_PK_RSA, GNUTLS_SEC_PARAM_NORMAL);
    d->errno = gnutls_x509_privkey_generate(d->key, GNUTLS_PK_RSA, bits, 0);

    return GNUTLS_E_SUCCESS == d->errno;
}

static QByteArray key_to_bytearray(gnutls_x509_privkey_t key, gnutls_x509_crt_fmt_t format, int *errno)
{
    QByteArray ba(4096, 0);
    size_t size = ba.size();

    *errno = gnutls_x509_privkey_export(key, format, ba.data(), &size);

    if (GNUTLS_E_SUCCESS != *errno)
        return QByteArray();

    ba.resize(size); // size has now been updated
    return ba;

}

QByteArray Key::toPEM()
{
    return key_to_bytearray(d->key, GNUTLS_X509_FMT_PEM, &d->errno);
}

QByteArray Key::toDER()
{
    return key_to_bytearray(d->key, GNUTLS_X509_FMT_DER, &d->errno);
}

