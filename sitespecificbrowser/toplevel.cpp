#include <qdebug.h>
#include <QtNetwork/QNetworkRequest>
#include <QtGui/QDesktopServices>
#include <qwebsettings.h>
#include <qwebframe.h>

#include "toplevel.h"

Page::Page()
    : QWebPage(0) 
{
}

bool Page::acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type )
{
    if ( type == QWebPage::NavigationTypeLinkClicked ) {
	QUrl base("http://mail.google.com/");
	if ( !base.isParentOf( request.url() ) ) {
	    qDebug() << "New browser for navigation to " << request.url();
	    QDesktopServices::openUrl( request.url() );
	    return false;
	}
    }

    return QWebPage::acceptNavigationRequest( frame, request, type );
}


TopLevel::TopLevel()
    : QWebView(0)
{
    m_page = new Page();
    setPage( m_page );

    QWebSettings::globalSettings()->setAttribute( QWebSettings::PluginsEnabled, true );
    QWebSettings::setIconDatabasePath( QString("./icons") );

    setWindowTitle( QString("GMail") );
    setUrl( QUrl("http://mail.google.com/") );

    connect( m_page->mainFrame(), SIGNAL( iconChanged() ), SLOT( iconLoaded() ) );
}

void TopLevel::iconLoaded()
{
    qDebug() << "Got icon";
    setWindowIcon( m_page->mainFrame()->icon() );
}
