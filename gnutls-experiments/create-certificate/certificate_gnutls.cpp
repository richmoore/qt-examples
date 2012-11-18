#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

extern "C" {
#include <gnutls/abstract.h>
};

#include <QString>
#include <QDateTime>
#include <QIODevice>

#include "key.h"
#include "certificaterequest.h"
#include "certificatebuilder.h"

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

struct CertificateBuilderPrivate
{
    int errno;
    gnutls_x509_crt_t crt;
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

//
// CertificateBuilder
//

CertificateBuilder::CertificateBuilder()
    : d(new CertificateBuilderPrivate)
{
    d->errno = gnutls_x509_crt_init(&d->crt);
}

CertificateBuilder::~CertificateBuilder()
{
    gnutls_x509_crt_deinit(d->crt);
    delete d;
}

int CertificateBuilder::error() const
{
    return d->errno;
}

QString CertificateBuilder::errorString() const
{
    return QString::fromUtf8(gnutls_strerror(d->errno));
}

bool CertificateBuilder::setRequest(const CertificateRequest *crq)
{
    d->errno = gnutls_x509_crt_set_crq (d->crt, crq->d->crq);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::setVersion(int version)
{
    d->errno = gnutls_x509_crt_set_version(d->crt, version);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::setSerial(const QByteArray &serial)
{
    d->errno = gnutls_x509_crt_set_serial(d->crt, serial.constData(), serial.size());
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::setActivationTime(const QDateTime &date)
{
    d->errno = gnutls_x509_crt_set_activation_time(d->crt, date.toTime_t());
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::setExpirationTime(const QDateTime &date)
{
    d->errno = gnutls_x509_crt_set_expiration_time(d->crt, date.toTime_t());
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::copyRequestExtensions(const CertificateRequest *crq)
{
    d->errno = gnutls_x509_crt_set_crq_extensions(d->crt, crq->d->crq);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::setBasicConstraints(bool ca, int pathLength)
{
    d->errno = gnutls_x509_crt_set_basic_constraints (d->crt, ca, pathLength);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::addKeyPurpose(KeyPurpose purpose, bool critical)
{
    QByteArray ba;

    switch(purpose) {
    case PurposeWebServer:
        ba = QByteArray(GNUTLS_KP_TLS_WWW_SERVER);
        break;
    default:
        qWarning("Unknown Purpose %d", int(purpose));
        return false;
    }

    return addKeyPurpose(ba, critical);
}

bool CertificateBuilder::addKeyPurpose(const QByteArray &oid, bool critical)
{
    d->errno = gnutls_x509_crt_set_key_purpose_oid(d->crt, oid.constData(), critical);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::setKeyUsage(KeyUsageFlags usages)
{
    uint usage = 0;
    if (usages & UsageEncipherOnly)
        usage |= GNUTLS_KEY_ENCIPHER_ONLY;
    if (usages & UsageCrlSign)
        usage |= GNUTLS_KEY_CRL_SIGN;
    if (usages & UsageKeyCertSign)
        usage |= GNUTLS_KEY_KEY_CERT_SIGN;
    if (usages & UsageKeyAgreement)
        usage |= GNUTLS_KEY_KEY_AGREEMENT;
    if (usages & UsageDataEncipherment)
        usage |= GNUTLS_KEY_DATA_ENCIPHERMENT;
    if (usages & UsageKeyEncipherment)
        usage |= GNUTLS_KEY_KEY_ENCIPHERMENT;
    if (usages & UsageNonRepudiation)
        usage |= GNUTLS_KEY_NON_REPUDIATION;
    if (usages & UsageDigitalSignature)
        usage |= GNUTLS_KEY_DIGITAL_SIGNATURE;
    if (usages & UsageDecipherOnly)
        usage |= GNUTLS_KEY_DECIPHER_ONLY;

    d->errno = gnutls_x509_crt_set_key_usage(d->crt, usage);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::addSubjectKeyIdentifier()
{
    QByteArray ba(128, 0); // Normally 20 bytes (SHA1)
    size_t size = ba.size();

    d->errno = gnutls_x509_crt_get_key_id(d->crt, 0, reinterpret_cast<unsigned char *>(ba.data()), &size);
    if (GNUTLS_E_SUCCESS != d->errno)
        return false;

    d->errno = gnutls_x509_crt_set_subject_key_id (d->crt, ba.constData(), size);
    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateBuilder::sign(const Key *key)
{
    gnutls_privkey_t abstractKey;
    d->errno = gnutls_privkey_init(&abstractKey);
    if (GNUTLS_E_SUCCESS != d->errno)
        return false;

    gnutls_privkey_import_x509(abstractKey, key->d->key, GNUTLS_PRIVKEY_IMPORT_AUTO_RELEASE);

    // TODO: self sign only for now
    d->errno = gnutls_x509_crt_privkey_sign(d->crt, d->crt, abstractKey, GNUTLS_DIG_SHA1, 0);
    return GNUTLS_E_SUCCESS == d->errno;
}

static QByteArray certificate_to_bytearray(gnutls_x509_crt_t crt, gnutls_x509_crt_fmt_t format, int *errno)
{
    QByteArray ba(4096, 0);
    size_t size = ba.size();

    *errno = gnutls_x509_crt_export(crt, format, ba.data(), &size);

    if (GNUTLS_E_SUCCESS != *errno)
        return QByteArray();

    ba.resize(size); // size has now been updated
    return ba;
}

QByteArray CertificateBuilder::toPEM()
{
    return certificate_to_bytearray(d->crt, GNUTLS_X509_FMT_PEM, &d->errno);
}

QByteArray CertificateBuilder::toDER()
{
    return certificate_to_bytearray(d->crt, GNUTLS_X509_FMT_DER, &d->errno);
}


