#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <animator.h>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    Animator *ann = new Animator();
    ann->resize(600, 600);

    QLabel kde(ann);

    QPixmap logo( QString("logo.png") );

    kde.setPixmap( logo );

    QSize ksz = kde.sizeHint();
    QPoint kstart( ann->width()-ksz.width(), 0 );
    QPoint kend( 0, ann->height()-ksz.height() );

    ann->addAnimation( &kde, "pos", kstart, kend );


    QLabel hello( ann );
    hello.setText( "<h1>Hello</h1>" );

    QSize sz = hello.sizeHint();

    QPoint start( 0, 0 );
    QPoint end( ann->width()-sz.width(),ann->height()-sz.height() );

    ann->addAnimation( &hello, "pos", start, end );

    QLabel world(ann);
    world.setText( "<font color=\"red\"><h1>World</h1></font>" );

    QSize wsz = world.sizeHint();
    QPoint wstart( 0, ann->height()-wsz.height() );
    QPoint wend( ann->width()-wsz.width(), 0 );

    ann->addAnimation( &world, "pos", wstart, wend );

    ann->show();
    ann->start();

    return app.exec();
}
