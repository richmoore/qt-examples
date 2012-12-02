#include <gnutls/gnutls.h>

#include <QByteArray>
#include <QFile>
#include <QDateTime>
#include <QSslKey>
#include <QSslCertificate>

#include "keybuilder.h"
#include "certificaterequestbuilder.h"
#include "certificaterequest.h"
#include "certificatebuilder.h"

int main(int argc, char **argv)
{
    // Start gnutls stuff
    gnutls_global_init();

    QSslKey key = KeyBuilder::generate( QSsl::Rsa, KeyBuilder::StrengthNormal );

    QFile k("output.key");
    k.open(QIODevice::WriteOnly);
    k.write(key.toPem());
    k.close();

    CertificateRequestBuilder reqbuilder;
    reqbuilder.setVersion(1);
    reqbuilder.setKey(key);
    reqbuilder.addNameEntry(CertificateRequestBuilder::EntryCountryName, "GB");
    reqbuilder.addNameEntry(CertificateRequestBuilder::EntryOrganizationName, "Westpoint");
    reqbuilder.addNameEntry(CertificateRequestBuilder::EntryOrganizationName, "West");
    reqbuilder.addNameEntry(CertificateRequestBuilder::EntryCommonName, "www.example.com");

    // Sign the request
    CertificateRequest req = reqbuilder.signedRequest(key);

    //
    // Export the results
    //
    QFile f("output.req");
    f.open(QIODevice::WriteOnly);
    f.write(req.toPem());
    f.close();

    //
    // Now make a certificate
    //
    CertificateBuilder builder;
    builder.setRequest(req);

    builder.setVersion(3);
    builder.setSerial("helloworld");
    builder.setActivationTime(QDateTime::currentDateTimeUtc());
    builder.setExpirationTime(QDateTime::currentDateTimeUtc());
    builder.setBasicConstraints(true);
    builder.setKeyUsage(CertificateBuilder::UsageCrlSign|CertificateBuilder::UsageKeyCertSign);
    builder.addSubjectKeyIdentifier();

    QSslCertificate cert = builder.signedCertificate(key);

    QFile c("output.crt");
    c.open(QIODevice::WriteOnly);
    c.write(cert.toPem());
    c.close();
}
