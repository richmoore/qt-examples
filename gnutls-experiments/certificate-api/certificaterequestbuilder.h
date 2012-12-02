// -*- c++ -*-

#ifndef CERTIFICATEREQUESTBUILDER_H
#define CERTIFICATEREQUESTBUILDER_H

#include <QSslKey>

#include "certificaterequest.h"

class QIODevice;
class QByteArray;

class CertificateRequestBuilder
{
public:
    enum EntryType {
        EntryCountryName,
        EntryOrganizationName,
        EntryOrganizationalUnitName,
        EntryCommonName
    };

    CertificateRequestBuilder();
    ~CertificateRequestBuilder();

    int error() const;
    QString errorString() const;

    bool setVersion(int version);
    int version() const;

    bool setKey(const QSslKey &key);

    bool addNameEntry(EntryType type, const QByteArray &value);
    bool addNameEntry(const QByteArray &oid, const QByteArray &value, bool raw=false);

    CertificateRequest signedRequest(const QSslKey &key);

private:
    struct CertificateRequestBuilderPrivate *d;
};

#endif // CERTIFICATEREQUESTBUILDER_H

