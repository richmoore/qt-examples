#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include <QByteArray>
#include <QFile>
#include <QDateTime>

#include "key.h"
#include "certificaterequest.h"
#include "certificatebuilder.h"

int main(int argc, char **argv)
{
    // Start gnutls stuff
    gnutls_global_init();

    Key key;
    key.generate( Key::KeyRsa, Key::StrengthNormal );

    CertificateRequest req;
    req.setVersion(1);
    req.setKey(&key);
    req.addNameEntry(CertificateRequest::EntryCountryName, "GB");
    req.addNameEntry(CertificateRequest::EntryOrganizationName, "Westpoint");
    req.addNameEntry(CertificateRequest::EntryOrganizationName, "West");
    req.addNameEntry(CertificateRequest::EntryCommonName, "www.example.com");

    // Sign the request
    req.sign(&key);

    //
    // Export the results
    //
    QFile f("output.req");
    f.open(QIODevice::WriteOnly);
    f.write(req.toPEM());
    f.close();

    QFile k("output.key");
    k.open(QIODevice::WriteOnly);
    k.write(key.toPEM());
    k.close();

    //
    // Now make a certificate
    //
    CertificateBuilder builder;
    builder.setRequest(&req);

    builder.setVersion(3);
    builder.setSerial("helloworld");
    builder.setActivationTime(QDateTime::currentDateTimeUtc());
    builder.setExpirationTime(QDateTime::currentDateTimeUtc());
    builder.setBasicConstraints(true);
    builder.setKeyUsage(CertificateBuilder::UsageCrlSign|CertificateBuilder::UsageKeyCertSign);
    builder.addSubjectKeyIdentifier();

    builder.sign(&key);

    QFile c("output.crt");
    c.open(QIODevice::WriteOnly);
    c.write(builder.toPEM());
    c.close();
}
