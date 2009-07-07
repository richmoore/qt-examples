#include <qapplication.h>
#include <qwidget.h>

#include <stdio.h>

#include "scriptui.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QString scriptName;
    QString uiName;

    if ( (argc != 2) && (argc != 3) ) {
	printf( "Usage: %s <nameprefix>\n"
                "       %s <uifile> <jsfile>\n",
		argv[0], argv[0] );
	return 0;
    }
    else if ( argc == 2 ) {
	QString prefix = QString::fromLocal8Bit( argv[1] );
	scriptName = QString("%1.js").arg(prefix);
	uiName = QString("%1.ui").arg(prefix);
    }
    else {
	uiName = QString::fromLocal8Bit( argv[1] );
	scriptName = QString::fromLocal8Bit( argv[2] );
    }

    ScriptUi ui;
    if ( !ui.loadScript( scriptName ) )
	return false;

    if ( !ui.loadUi( uiName ) )
	return false;

    ui.connectScript();

    ui.view()->show();
    return app.exec();
}
