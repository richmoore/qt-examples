#ifndef FOO_H

#include <QDebug>

#include "foo.h"

Foo::Foo()
{
}

Foo::~Foo()
{
}

QString Foo::myString()
{
    return m_mystring;
}

void Foo::setMyString( const QString &str )
{
    m_mystring = str;
}

void Foo::notASlot()
{
    qDebug() << "Hey, I'm not a slot";
}

void Foo::reverseString()
{
    qDebug() << "Reverse wasn't implemented";
    qDebug() << "String is " << m_mystring;
    // #####
}

#endif // FOO_H
