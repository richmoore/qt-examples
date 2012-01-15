#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include <gnutls/gnutls.h>

#include "sslsocket.h"
#include "getslash.h"

static void
tls_log_func (int level, const char *str)
{
  fprintf (stderr, "|<%d>| %s", level, str);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        qDebug() << "Usage:" << argv[0] << "hostname";
        return 1;
    }

    QCoreApplication app(argc, argv);

    // Start gnutls stuff
    gnutls_global_set_log_function(tls_log_func);
    //gnutls_global_set_log_level(5);
    gnutls_global_init();

    SslSocket sock;
    sock.connectToHost(QString(argv[1]), 443);

    GetSlash getter(&sock);

    return app.exec();
}
