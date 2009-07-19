#include <qlabel.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qurl.h>

#include "kwebthumbnailer.h"

#include "toplevel.h"


TopLevel::TopLevel()
    : QWidget()
{
    edit = new QLineEdit( this );
    view = new QLabel( this );
    view->setFixedSize( QSize(300,400) );
    view->setText( "Ready..." );
    connect(edit, SIGNAL(returnPressed()), SLOT(reload()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(edit);
    layout->addWidget(view);

    thumbnailer = new KWebThumbnailer( this );
    thumbnailer->setZoomFactory( 0.4 );
    thumbnailer->setSize( QSize(300,400) );
    thumbnailer->setUrl( QUrl("http://www.kde.org/") );
    connect( thumbnailer, SIGNAL(done(bool)), SLOT(completed(bool)) );
    view->setText( "Working..." );
    thumbnailer->start();
}

void TopLevel::reload()
{
    thumbnailer->setUrl( QUrl( edit->text() ) );
    view->setText("Working...");
    thumbnailer->start();
}

void TopLevel::completed( bool success )
{
    if ( !success ) {
	view->setText("Failed");
	return;
    }

    view->setPixmap( thumbnailer->thumbnail() );
}
