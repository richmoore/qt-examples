// -*- c++ -*-

#ifndef CERTIFICATEREQUEST_H
#define CERTIFICATEREQUEST_H

#include <QByteArray>

class QIODevice;

class Key;

class CertificateRequest
{
public:
    enum RequestFormat {
        FormatPem,
        FormatDer
    };

    enum EntryType {
        EntryCountryName,
        EntryOrganizationName,
        EntryOrganizationalUnitName,
        EntryCommonName
    };

    CertificateRequest();
    CertificateRequest(QIODevice *io, RequestFormat format=FormatPem);
    ~CertificateRequest();

    int error() const;
    QString errorString() const;

    bool setVersion(int version);
    int version() const;

    bool setKey(const Key *key);

    bool addNameEntry(EntryType type, const QByteArray &value);
    bool addNameEntry(const QByteArray &oid, const QByteArray &value, bool raw=false);

    bool sign(const Key *key);

    QByteArray toPEM();
    QByteArray toDER();

private:
    struct CertificateRequestPrivate *d;
    friend class CertificateBuilder;
};

#endif // CERTIFICATEREQUEST_H

