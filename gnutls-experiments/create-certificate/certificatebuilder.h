// -*- c++ -*-

#ifndef CERTIFICATEBUILDER_H
#define CERTIFICATEBUILDER_H

#include <QString>
#include <QFlags>

class QDateTime;

class CertificateRequest;

class CertificateBuilder
{
public:
    enum CertificateFormat {
        FormatPem,
        FormatDer
    };

    enum KeyPurpose {
        PurposeWebServer
    };

    enum KeyUsageFlag {
        UsageEncipherOnly,
        UsageCrlSign,
        UsageKeyCertSign,
        UsageKeyAgreement,
        UsageDataEncipherment,
        UsageKeyEncipherment,
        UsageNonRepudiation,
        UsageDigitalSignature,
        UsageDecipherOnly
    };
    Q_DECLARE_FLAGS(KeyUsageFlags, KeyUsageFlag)

    CertificateBuilder();
    ~CertificateBuilder();

    int error() const;
    QString errorString() const;

    bool setRequest(const CertificateRequest *crq);

    bool setVersion(int version=3);
    bool setSerial(const QByteArray &serial);

    bool setActivationTime(const QDateTime &date);
    bool setExpirationTime(const QDateTime &date);

    // Extensions

    bool copyRequestExtensions(const CertificateRequest *crq);
    bool setBasicConstraints(bool ca=false, int pathLength=-1);

    // Extended usage
    bool addKeyPurpose(KeyPurpose purpose, bool critical=false);
    bool addKeyPurpose(const QByteArray &oid, bool critical=false);

    // Usage
    bool setKeyUsage(KeyUsageFlags usage);

    bool addSubjectKeyIdentifier();

    //bool addAuthorityKeyIdentifier(); // TODO: need cert absraction

    bool sign(const Key *key);

    QByteArray toPEM();
    QByteArray toDER();

private:
    struct CertificateBuilderPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CertificateBuilder::KeyUsageFlags)

#endif // CERTIFICATEBUILDER_H
