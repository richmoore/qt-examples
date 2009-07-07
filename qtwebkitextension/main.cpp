#include <qapplication.h>
#include <qwebview.h>

#include "testobject.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QWebView *view = new QWebView();

    MyApi *myapi = new MyApi( view );
    myapi->setWebView( view );

    view->setUrl( QUrl( "test.html" ) );
    view->show();

    return app.exec();
}

