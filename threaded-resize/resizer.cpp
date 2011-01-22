#include <QImage>

#include "resizer.h"

struct ResizerPrivate
{
    QSize size;
    Qt::AspectRatioMode aspectMode;
    QImage input;
    QString inputFilename;
};

Resizer::Resizer( QObject *parent )
    : QObject(parent),
      d( new ResizerPrivate )
{
    d->aspectMode = Qt::KeepAspectRatio;
}

Resizer::~Resizer()
{
    delete d;
}

void Resizer::setSize( const QSize &size )
{
    d->size = size;
}

void Resizer::setAspectRatioMode( const Qt::AspectRatioMode mode )
{
    d->aspectMode = mode;
}
    
void Resizer::setInput( const QImage &input )
{
    d->input = input;
}

void Resizer::setInput( const QString &filename )
{
    d->inputFilename = filename;
}

void Resizer::start()
{
    if  ( !d->inputFilename.isEmpty() ) {
        d->input.load( d->inputFilename );
    }

    if ( d->input.isNull() ) {
        emit error();
        return;       
    }

    QImage output = d->input.scaled( d->size, d->aspectMode, Qt::SmoothTransformation );
    emit finished( output );
}


