#include <QIODevice>

#include "certificaterequest_p.h"

#include "certificaterequestbuilder_p.h"

static int qsslkey_to_key(const QSslKey &qkey, gnutls_x509_privkey_t key)
{
    QByteArray buf(qkey.toPem());

    // Setup a datum
    gnutls_datum_t buffer;
    buffer.data = (unsigned char *)(buf.data());
    buffer.size = buf.size();

    return gnutls_x509_privkey_import(key, &buffer, GNUTLS_X509_FMT_PEM);
}

CertificateRequestBuilder::CertificateRequestBuilder()
    : d(new CertificateRequestBuilderPrivate)
{
    gnutls_x509_crq_init(&d->crq);
    d->errno = GNUTLS_E_SUCCESS;
}

CertificateRequestBuilder::~CertificateRequestBuilder()
{
    gnutls_x509_crq_deinit(d->crq);
    delete d;
}

int CertificateRequestBuilder::error() const
{
    return d->errno;
}

QString CertificateRequestBuilder::errorString() const
{
    return QString::fromUtf8(gnutls_strerror(d->errno));
}

bool CertificateRequestBuilder::setVersion(int version)
{
    d->errno = gnutls_x509_crq_set_version(d->crq, version);
    return GNUTLS_E_SUCCESS == d->errno;
}

int CertificateRequestBuilder::version() const
{
    int ver = gnutls_x509_crq_get_version(d->crq);
    if (ver < 0)
        d->errno = ver;
    return ver;
}

bool CertificateRequestBuilder::setKey(const QSslKey &qkey)
{
    gnutls_x509_privkey_t key;
    gnutls_x509_privkey_init(&key);

    d->errno = qsslkey_to_key(qkey, key);
    if (GNUTLS_E_SUCCESS != d->errno) {
        gnutls_x509_privkey_deinit(key);
        return false;
    };

    d->errno = gnutls_x509_crq_set_key(d->crq, key);

    gnutls_x509_privkey_deinit(key);

    return GNUTLS_E_SUCCESS == d->errno;
}

bool CertificateRequestBuilder::addNameEntry(EntryType type, const QByteArray &value)
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

bool CertificateRequestBuilder::addNameEntry(const QByteArray &oid, const QByteArray &value, bool raw)
{
    d->errno = gnutls_x509_crq_set_dn_by_oid(d->crq, oid.constData(), raw,
                                             value.constData(), qstrlen(value.constData()));
    return GNUTLS_E_SUCCESS == d->errno;   
}

CertificateRequest CertificateRequestBuilder::signedRequest(const QSslKey &qkey)
{
    CertificateRequest result;

    gnutls_x509_privkey_t key;
    gnutls_x509_privkey_init(&key);

    d->errno = qsslkey_to_key(qkey, key);
    if (GNUTLS_E_SUCCESS != d->errno) {
        gnutls_x509_privkey_deinit(key);
        return result;
    };

    d->errno = gnutls_x509_crq_sign2(d->crq, key, GNUTLS_DIG_SHA1, 0);
    gnutls_x509_privkey_deinit(key);

    if (GNUTLS_E_SUCCESS != d->errno)
        return result;

    gnutls_x509_crq_t crqsave = result.d->crq;
    result.d->crq = d->crq;
    d->crq = crqsave;

    return result;
}

