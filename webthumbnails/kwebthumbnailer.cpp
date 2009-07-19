#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QSize>
#include <QtGui/QPainter>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>

#include "kwebthumbnailer.h"

class KWebThumbnailerPrivate
{
public:
    KWebThumbnailerPrivate() : page(0) {
    }

    QWebPage *page;
    QPixmap thumbnail;
    QSize size;
    QUrl url;
    double zoom;
};

KWebThumbnailer::KWebThumbnailer( QObject *parent )
    : QObject( parent ),
      d( new KWebThumbnailerPrivate )
{
}

KWebThumbnailer::KWebThumbnailer( const QUrl &url, const QSize &size, QObject *parent )
    : QObject( parent ),
      d( new KWebThumbnailerPrivate )
{
    d->url = url;
    d->size = size;
}

KWebThumbnailer::~KWebThumbnailer()
{
    delete d;
}

void KWebThumbnailer::setUrl( const QUrl &url )
{
    d->url = url;
}

void KWebThumbnailer::setSize( const QSize &size )
{
    d->size = size;
}

void KWebThumbnailer::setZoomFactory( double zoom )
{
    d->zoom = zoom;
}

void KWebThumbnailer::start()
{
    d->page = new QWebPage( this );
    d->page->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    d->page->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );
    d->page->mainFrame()->setZoomFactor( d->zoom );
    d->page->setViewportSize( d->size );

    d->page->mainFrame()->load( d->url );

    connect( d->page, SIGNAL(loadFinished(bool)), this, SLOT(completed(bool)) );

    qDebug() << "Started..." << endl;
}

void KWebThumbnailer::completed( bool success )
{
    qDebug() << "Completed, " << success << endl;
    if ( !success ) {
	delete d->page;
	d->page = 0;
	d->thumbnail = QPixmap();
	emit done(false);

	return;
    }

    d->thumbnail = QPixmap( d->size );

    QPainter paint( &d->thumbnail );
    d->page->mainFrame()->render( &paint, QRegion( QRect(QPoint(0,0), d->size) ) );
    emit done(success);
}

QPixmap KWebThumbnailer::thumbnail() const
{
    return d->thumbnail;
}

bool KWebThumbnailer::isValid() const
{
    return d->thumbnail.isNull();
}


