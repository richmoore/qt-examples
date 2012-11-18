// -*- c++ -*-

#ifndef KEY_H
#define KEY_H

#include <QByteArray>

class Key
{
public:
    enum KeyType {
        KeyRsa
    };

    enum KeyStrength {
        StrengthNormal
    };

    Key();
    ~Key();

    bool generate( KeyType type, KeyStrength strength );

    QByteArray toPEM();
    QByteArray toDER();

private:
    struct KeyPrivate *d;
    friend class CertificateRequest;
    friend class CertificateBuilder;
};

#endif // KEY_H
