#include <QDateTime>
#include <QSslKey>

extern "C" {
#include <gnutls/abstract.h>
};

#include "certificaterequest_p.h"

#include "certificatebuilder_p.h"

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

static int qsslkey_to_key(const QSslKey &qkey, gnutls_x509_privkey_t key)
{
    QByteArray buf(qkey.toPem());

    // Setup a datum
    gnutls_datum_t buffer;
    buffer.data = (unsigned char *)(buf.data());
    buffer.size = buf.size();

    return gnutls_x509_privkey_import(key, &buffer, GNUTLS_X509_FMT_PEM);
}

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

bool CertificateBuilder::setRequest(const CertificateRequest &crq)
{
    d->errno = gnutls_x509_crt_set_crq(d->crt, crq.d->crq);
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

bool CertificateBuilder::copyRequestExtensions(const CertificateRequest &crq)
{
    d->errno = gnutls_x509_crt_set_crq_extensions(d->crt, crq.d->crq);
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

QSslCertificate CertificateBuilder::signedCertificate(const QSslKey &qkey)
{
    gnutls_x509_privkey_t key;
    gnutls_x509_privkey_init(&key);

    d->errno = qsslkey_to_key(qkey, key);
    if (GNUTLS_E_SUCCESS != d->errno) {
        gnutls_x509_privkey_deinit(key);
        return QSslCertificate();
    };

    gnutls_privkey_t abstractKey;
    d->errno = gnutls_privkey_init(&abstractKey);
    if (GNUTLS_E_SUCCESS != d->errno) {
        gnutls_x509_privkey_deinit(key);
        return QSslCertificate();
    }

    gnutls_privkey_import_x509(abstractKey, key, GNUTLS_PRIVKEY_IMPORT_AUTO_RELEASE);

    // TODO: self sign only for now
    d->errno = gnutls_x509_crt_privkey_sign(d->crt, d->crt, abstractKey, GNUTLS_DIG_SHA1, 0);

    gnutls_x509_privkey_deinit(key);

    if (GNUTLS_E_SUCCESS != d->errno)
        return QSslCertificate();

    QByteArray buffer = certificate_to_bytearray(d->crt, GNUTLS_X509_FMT_PEM, &d->errno);
    return QSslCertificate(buffer);
}

