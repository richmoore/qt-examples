#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>
#include <QAbstractSocket>

class QSslError;
class QSslCertificate;
class QSslCipher;

class Connector : public QObject
{
    Q_OBJECT
public:
    Connector( const QString &host, int port );
    ~Connector();

    void connectToHost();

    void dumpCertificate( const QSslCertificate &cert );
    void dumpCipher( const QSslCipher &cipher );

public slots:
    void ready();
    void socketError( QAbstractSocket::SocketError error );
    void sslError( const QList<QSslError> &errors );

private:
    struct ConnectorPrivate *d;
};

#endif // CONNECTOR_H
