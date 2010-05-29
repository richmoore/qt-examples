#include <QApplication>
#include <QWebView>

#include "whitelistnetworkaccessmanager.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    QWebView view;

    WhiteListNetworkAccessManager proxy;
    proxy.addDomain( QString("www.kde.org") );
    view.page()->setNetworkAccessManager( &proxy );

    view.setUrl( QUrl("http://www.kde.org/") );
    view.show();

    return app.exec();
}

