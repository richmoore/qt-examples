
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include "keybuilder.h"

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

QSslKey KeyBuilder::generate( QSsl::KeyAlgorithm algo, KeyStrength strength )
{
    gnutls_x509_privkey_t key;
    gnutls_x509_privkey_init(&key);

    // TODO: type, strength
    uint bits = gnutls_sec_param_to_pk_bits(GNUTLS_PK_RSA, GNUTLS_SEC_PARAM_NORMAL);

    int errno = gnutls_x509_privkey_generate(key, GNUTLS_PK_RSA, bits, 0);
    if (GNUTLS_E_SUCCESS != errno) {
        qWarning("Failed to generate key %s", gnutls_strerror(errno));
        gnutls_x509_privkey_deinit(key);
        return QSslKey();
    }

    QByteArray ba = key_to_bytearray(key, GNUTLS_X509_FMT_PEM, &errno);
    gnutls_x509_privkey_deinit(key);

    if (GNUTLS_E_SUCCESS != errno) {
        qWarning("Failed to convert key to bytearray %s", gnutls_strerror(errno));
        return QSslKey();
    }
    

    return QSslKey(ba, algo, QSsl::Pem);
}
