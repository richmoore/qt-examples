#include <QCoreApplication>

#include "httpserver.h"
#include "httpsserver.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    HttpServer server(8080);
    HttpsServer sslserver(8443);

    return app.exec();
}

