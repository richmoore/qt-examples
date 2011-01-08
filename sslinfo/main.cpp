#include <QCoreApplication>
#include <QTextStream>
#include <stdio.h>

#include "connector.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    int port;
    QString host;

    if ( argc == 2 ) {
        host = QString::fromLocal8Bit( argv[1] );
        port = 443;
    }
    else if ( argc == 3 ) {
        host = QString::fromLocal8Bit( argv[1] );
        port = QString::fromLocal8Bit( argv[2] ).toInt();
    }
    else {
        QTextStream out(stdout);
        out << "Usage: " << argv[0] << " host <port>" << endl;
        return 1;
    }

    Connector con( host, port );
    con.connectToHost();

    return app.exec();
}
