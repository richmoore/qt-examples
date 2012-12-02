// -*- c++ -*-

#ifndef KEYBUILDER_H
#define KEYBUILDER_H

#include <QtNetwork/QSslKey>
#include <QtNetwork/QSsl>

class KeyBuilder
{
public:
    enum KeyStrength {
        StrengthNormal
    };

    static QSslKey generate( QSsl::KeyAlgorithm algo, KeyStrength strength );

private:
    KeyBuilder() {}
    ~KeyBuilder() {}

private:
    struct KeyBuilderPrivate *d;
};

#endif // KEYBUILDER_H
