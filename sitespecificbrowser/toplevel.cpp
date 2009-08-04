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

/** Stuff that should be per-webapp
struct WebAppOptions
{
    QUrl startUrl;
    QList<QUrl> allowedBases;
    QIcon windowIcon;
    QString windowTitle;
    QMap<QString, QString> jsActions;
};
*/

TopLevel::TopLevel()
    : QWebView(0)
{
    m_page = new Page();
    setPage( m_page );

    QWebSettings::globalSettings()->setAttribute( QWebSettings::PluginsEnabled, true );
    QWebSettings::setIconDatabasePath( QString("./icons") );
    QIcon icon = QWebSettings::iconForUrl( QUrl("http://mail.google.com/") );
    qDebug() << icon;

    setWindowTitle( QString("GMail") );
    setUrl( QUrl("http://mail.google.com/") );

    connect( m_page->mainFrame(), SIGNAL( iconChanged() ), SLOT( iconLoaded() ) );
}

void TopLevel::iconLoaded()
{
    qDebug() << "Got icon";
    qDebug() << m_page->mainFrame()->icon().isNull();
    setWindowIcon( m_page->mainFrame()->icon() );

//    QIcon icon = QWebSettings::iconForUrl( QUrl("http://mail.google.com/") );
//    qDebug() << icon;
}
