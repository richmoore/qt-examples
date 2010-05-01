#include <QApplication>
#include <QWebView>

#include "networkaccessmanagerproxy.h"
#include "networkaccessviewer.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    QWebView view;

    NetworkAccessManagerProxy proxy;
    NetworkAccessViewer viewer;
    proxy.setNetworkAccessViewer( &viewer );

    view.page()->setNetworkAccessManager( &proxy );

    view.setUrl( QUrl("http://www.kde.org/") );
    view.show();

    return app.exec();
}
