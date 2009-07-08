#include <qapplication.h>
#include <qwebview.h>
#include <qwebsettings.h>

#include "pluginfactory.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QWebSettings::globalSettings()->setAttribute( QWebSettings::PluginsEnabled, true );

    QWebView *view = new QWebView();
    PluginFactory *factory = new PluginFactory( view );
    view->page()->setPluginFactory( factory );

    view->setUrl( QUrl( "test.html" ) );
    view->show();

    return app.exec();
}

