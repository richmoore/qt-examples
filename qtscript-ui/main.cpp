#include <qapplication.h>
#include <qwidget.h>

#include "scriptui.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    ScriptUi ui;
    if ( !ui.loadScript( "converter.js") )
	return false;

    if ( !ui.loadUi( "converter.ui") )
	return false;

    ui.connectScript();

    ui.view()->show();
    return app.exec();
}
