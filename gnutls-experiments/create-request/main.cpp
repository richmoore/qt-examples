#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#include <QByteArray>
#include <QFile>

#include "key.h"
#include "certificaterequest.h"

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
}
