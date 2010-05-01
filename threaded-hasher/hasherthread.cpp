#include <QFile>
#include "hasherthread.h"

HasherThread::HasherThread(QObject *parent) :
    QThread(parent)
{
    hasher = new QCryptographicHash( QCryptographicHash::Sha1 );
}

HasherThread::~HasherThread()
{
    delete hasher;
}

void HasherThread::setFilename( const QString &filename )
{
    this->filename = filename;
}

void HasherThread::setAlgorithm( QCryptographicHash::Algorithm alg )
{
    delete hasher;
    hasher = new QCryptographicHash( alg );
}

void HasherThread::run()
{
    QFile f( filename );
    if ( !f.open(QIODevice::ReadOnly) ) {
        emit error( QString("Unable to open file %1").arg(filename) );
        return;
    }

    hasher->reset();

    char buffer[16*1024];
    qint64 count;
    do {
        count = f.read( buffer, sizeof(buffer) );
        if ( count == -1 ) {
            emit error( QString("Read error") );
            break;
        }
        hasher->addData( buffer, count );
    } while( !f.atEnd() );

    emit completed( hasher->result().toHex() );
}
