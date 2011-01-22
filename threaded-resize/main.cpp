#include <QApplication>
#include <QThread>

#include <stdio.h>

#include "resizer.h"
#include "viewer.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    if ( argc != 2 ) {
        printf( "Usage: %s <filename>\n", argv[0] );
        return 1;
    }

    Viewer *view = new Viewer();

    QThread *thread = new QThread();
    Resizer *resizer = new Resizer();
    resizer->moveToThread( thread );

    QObject::connect( thread, SIGNAL(started()), resizer, SLOT(start()) );
    QObject::connect( resizer, SIGNAL(finished(const QImage &)), view, SLOT(setImage(const QImage &)) );

    view->show();

    resizer->setInput( QString::fromLocal8Bit(argv[1]) );
    resizer->setSize( QSize(400,400) );

    thread->start();

    return app.exec();
}
