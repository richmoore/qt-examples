#ifndef FOO_H
#define FOO_H

#include <qobject.h>

class Foo : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString myString READ myString WRITE setMyString )

public:
    Foo();
    ~Foo();

    QString myString();
    void setMyString( const QString &str );

    Q_INVOKABLE void notASlot();

public slots:
    void reverseString();

private:
    QString m_mystring;
};

#endif // FOO_H
