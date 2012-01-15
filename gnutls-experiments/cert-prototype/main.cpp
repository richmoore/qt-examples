#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <qfile.h>
#include <qdebug.h>
#include <qssl.h>
#include <qdatetime.h>

//
// Utils that appear to be pretty much as needed.
//

static gnutls_x509_crt_fmt_t qSslEncodingFormatToCrtFmt(QSsl::EncodingFormat format)
{
    if (format == QSsl::Pem)
        return GNUTLS_X509_FMT_PEM;
    else
        return GNUTLS_X509_FMT_DER;
}

static gnutls_x509_crt_t x509_cert_from_qbytearray(const QByteArray &buf, QSsl::EncodingFormat format=QSsl::Pem)
{
    // Setup a datum
    gnutls_datum_t buffer;
    buffer.data = (unsigned char *)(buf.data());
    buffer.size = buf.size();

    // Create the cert object
    gnutls_x509_crt_t cert;
    int result = gnutls_x509_crt_init(&cert);
    if (result < 0)
        return 0;

    // Import the cert
    gnutls_x509_crt_fmt_t certformat = qSslEncodingFormatToCrtFmt(format);
    result = gnutls_x509_crt_import(cert, &buffer, certformat );
    if (result < 0) {
        gnutls_x509_crt_deinit(cert);
        return 0;
    }

    return cert;
}

static gnutls_x509_crt_t x509_cert_from_file(const QString &filename, QSsl::EncodingFormat format=QSsl::Pem)
{
    // Read the data
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
        return 0;

    QByteArray buf = f.readAll();
    f.close();

    return x509_cert_from_qbytearray(buf, format);
}

static QByteArray x509_cert_to_text(const gnutls_x509_crt_t cert)
{
    gnutls_datum_t cert_info;
    gnutls_x509_crt_print(cert, GNUTLS_CRT_PRINT_FULL, &cert_info);

    QByteArray result(reinterpret_cast<char *>(cert_info.data), cert_info.size);
    gnutls_free(cert_info.data);

    return result;
}

static QDateTime x509_cert_get_start(const gnutls_x509_crt_t cert)
{
    QDateTime result;
    time_t time = gnutls_x509_crt_get_activation_time(cert);
    if (time == -1)
        return result;

    result.setTime_t(time);
    return result.toUTC();
}

static QDateTime x509_cert_get_expiry(const gnutls_x509_crt_t cert)
{
    QDateTime result;
    time_t time = gnutls_x509_crt_get_expiration_time(cert);
    if (time == -1)
        return result;

    result.setTime_t(time);
    return result.toUTC();
}

static bool load_x509_cert_list(gnutls_x509_crt_t **certs, unsigned int *size, const QString &filename)
{
    qDebug() << "load_x509_cert_list" << filename;

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open cert file" << filename;
        return false;
    }

    QByteArray buf = f.readAll();
    f.close();

    // Setup a datum
    gnutls_datum_t buffer;
    buffer.data = (unsigned char *)(buf.data());
    buffer.size = buf.size();

    int flags = 0; // We don't want to fail if unsorted, and our buffer can't be too short
    int result = gnutls_x509_crt_list_import2(certs, size, &buffer, GNUTLS_X509_FMT_PEM, flags);
    
    if (result < 0) {
        qDebug() << "Failed to import cert list" << gnutls_strerror(result);
        return false;
    }

    return true;
}

//
// Stuff that needs more work.
//

static void x509_cert_subject_element(const gnutls_x509_crt_t cert, const char *oid)
{
    char buf[100]; // TODO: Use a qbytearray
    size_t size = sizeof(buf);

    int index=0;
    do {
        int result = gnutls_x509_crt_get_dn_by_oid(cert, oid, index, false, buf, &size);
        if (result == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
            break;
        if (result != 0) {
            qDebug() << "failed at index" << index << " result was " << result;
            break;
        }
        qDebug() << "value:" << buf;

        index++;
        size = sizeof(buf);
    } while(true);
}

static void x509_cert_subject(const gnutls_x509_crt_t cert)
{
    char oid[100]; // TODO: Use a qbytearray
    size_t size = sizeof(oid);

    int index=0;
    do {
        int result = gnutls_x509_crt_get_dn_oid(cert, index, oid, &size);
        if (result == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
            break;
        if (result != 0) {
            qDebug() << "failed at index" << index << " result was " << result;
            break;
        }
        qDebug() << "Subject oid:" << oid;
        
        x509_cert_subject_element(cert, oid);
        index++;
        size = sizeof(oid);
    } while(true);
}

//
// Test harness.
//

int main(int argc, char **argv)
{
    if (argc < 2) {
        qDebug() << "Usage:" << argv[0] << "<cert.pem>";
        return 1;
    }

    // Start gnutls stuff
    gnutls_global_init();

    gnutls_x509_crt_t cert = x509_cert_from_file(QString(argv[1]));

    QByteArray text = x509_cert_to_text(cert);
    qDebug() << text;

    qDebug() << "Start Date:" << x509_cert_get_start(cert);
    qDebug() << "Expiry Date:" << x509_cert_get_expiry(cert);

    x509_cert_subject(cert);
}
