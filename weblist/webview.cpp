#include <QtGui/QStyleOptionGraphicsItem>

#include <qlistmodelinterface.h>
#include "qdebug.h"
#include "webview.h"

WebViewItem::WebViewItem( int index, QtGraphicsListView *view )
    : QtGraphicsListViewItem( index, view )
{
    qDebug() << "Custom item";
    page = new QWebPage();
    page->mainFrame()->setZoomFactor( 0.4 );
    page->mainFrame()->setHtml("<html><body>Loading...</body></html>");
    page->mainFrame()->setScrollBarPolicy( Qt::Horizontal, Qt::ScrollBarAlwaysOff );
    page->mainFrame()->setScrollBarPolicy( Qt::Vertical, Qt::ScrollBarAlwaysOff );

    // Yucky dance to get data from the model
    QList<int> roles;
    roles.append( QtListModelInterface::TextRole );
    QString url = view->model()->data(index, roles)[QtListModelInterface::TextRole].toString();

    page->mainFrame()->load( QUrl(url) );
    connect( page, SIGNAL(loadFinished(bool)), this, SLOT(updateItem()) );

    page->setViewportSize( QSize(300, 300));

}

void WebViewItem::updateItem()
{
    itemChanged();
}

void WebViewItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    page->mainFrame()->render(painter, option->rect);
}

WebView::WebView(Qt::Orientation orientation, QGraphicsWidget *parent, Qt::WindowFlags wFlags)
    : QtGraphicsListView( orientation, parent, wFlags ),
      mEnablePreviews( true )
{
}

WebView::~WebView()
{
}

void WebView::setEnablePreviews( bool enable )
{
    mEnablePreviews = enable;
}

QSizeF WebView::itemSize(const QStyleOptionViewItemV4 *option, int index) const
{
    if ( mEnablePreviews )
	return QSizeF( 300.0, 300.0 );
    
    return QtGraphicsListView::itemSize(option, index);
}


QtGraphicsListViewItem *WebView::newViewItem(int index)
{
    if ( mEnablePreviews )
	return new WebViewItem( index, this );
    else
	return QtGraphicsListView::newViewItem( index );
}

