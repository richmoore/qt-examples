#include <qapplication.h>
#include <qfile.h>
#include <stdio.h>
#include <stdlib.h>

#include "proxyautoconfig.h"

int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    ProxyAutoConfig pac;

    if ( argc != 4 ) {
	printf( "Usage: %s pac-file url host\n", argv[0] );
	exit(0);
    }

    QFile f(argv[1]);
    f.open( QFile::ReadOnly );
    QString config = f.readAll();

    pac.setConfig( config );

    QString result = pac.findProxyForUrl( argv[2], argv[3] );
    printf( "%s\n", qPrintable(result) );
}
