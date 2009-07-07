#include <qapplication.h>
#include <qwidget.h>

#include <stdio.h>

#include "scriptui.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    if ( argc != 2 ) {
	printf( "Usage: %s <nameprefix>\n", argv[0] );
	return 0;
    }

    QString prefix = QString::fromLocal8Bit( argv[1] );

    ScriptUi ui;
    if ( !ui.loadScript( QString("%1.js").arg(prefix) ) )
	return false;

    if ( !ui.loadUi( QString("%1.ui").arg(prefix) ) )
	return false;

    ui.connectScript();

    ui.view()->show();
    return app.exec();
}
