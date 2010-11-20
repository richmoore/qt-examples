#include <QApplication>
#include <QSslSocket>
#include <QDebug>

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QSslSocket socket;
    // This is the normal way to specify the host, but Qt does the DNS lookup
    //socket.connectToHostEncrypted( "bugs.kde.org", 443 );

    // If we specify it like this we get a certificate mismatch
    //socket.connectToHostEncrypted( "138.246.255.179", 443 );

    // This way we can do the DNS lookup and also specify the peer we want to connect to
    socket.connectToHostEncrypted( "138.246.255.179", 443, "bugs.kde.org" );
    if ( !socket.waitForEncrypted() ) {
        qDebug() << socket.errorString();
        return 1;
    }

    socket.write( "GET / HTTP/1.1\r\n" \
                  "Host: bugs.kde.org\r\n" \
                  "Connection: Close\r\n\r\n" );
    while ( socket.waitForReadyRead() ) {
        qDebug() <<  socket.readAll().data();
    };

    qDebug() << "Done";

    return 0;
}
