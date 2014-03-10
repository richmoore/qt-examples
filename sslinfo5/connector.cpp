#include <QCoreApplication>
#include <QDebug>
#include <QSslSocket>
#include <QSslCipher>
#include <QDateTime>

#include "connector.h"

struct ConnectorPrivate
{
    QString host;
    int port;
    QSslSocket *sock;
};

Connector::Connector( const QString &host, int port )
{
    d = new ConnectorPrivate;
    d->host = host;
    d->port = port;
    d->sock = 0;
}

Connector::~Connector()
{
    if (d->sock)
        delete d->sock;
    delete d;
}

void Connector::connectToHost()
{
    dumpSslInfo();

    qDebug() << "Connecting...";

    d->sock = new QSslSocket(this);
    connect( d->sock, SIGNAL(encrypted()), this, SLOT(ready()) );
    connect( d->sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)) );
    connect( d->sock, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslError(const QList<QSslError> &)) );

    d->sock->connectToHostEncrypted( d->host, d->port );
}

void Connector::dumpSslInfo()
{
    qDebug() << "Supports SSL:\t\t" << QSslSocket::supportsSsl();
    qDebug() << "Run-time Library:\t" << QSslSocket::sslLibraryVersionString();
}

void Connector::dumpCertificate( const QSslCertificate &cert )
{
    qDebug() << cert.toPem();

    qDebug() << "== Subject Info ==\b";
    qDebug() << "CommonName:\t\t" << cert.subjectInfo( QSslCertificate::CommonName );
    qDebug() << "Organization:\t\t" << cert.subjectInfo( QSslCertificate::Organization );
    qDebug() << "LocalityName:\t\t" << cert.subjectInfo( QSslCertificate::LocalityName );
    qDebug() << "OrganizationalUnitName:\t" << cert.subjectInfo( QSslCertificate::OrganizationalUnitName );
    qDebug() << "StateOrProvinceName:\t" << cert.subjectInfo( QSslCertificate::StateOrProvinceName );

    QMultiMap<QSsl::AlternativeNameEntryType, QString> altNames = cert.subjectAlternativeNames();
    if ( !altNames.isEmpty() ) {
        qDebug() << "Subject Alternate Names (DNS):";
        foreach (const QString &altName, altNames.values(QSsl::DnsEntry)) {
            qDebug() << altName;
        }

        qDebug() << "Subject Alternate Names (Email):";
        foreach (const QString &altName, altNames.values(QSsl::EmailEntry)) {
            qDebug() << altName;
        }
    }

    qDebug() << "\n== Issuer Info ==";
    qDebug() << "CommonName:\t\t" << cert.issuerInfo( QSslCertificate::CommonName );
    qDebug() << "Organization:\t\t" << cert.issuerInfo( QSslCertificate::Organization );
    qDebug() << "LocalityName:\t\t" << cert.issuerInfo( QSslCertificate::LocalityName );
    qDebug() << "OrganizationalUnitName:\t" << cert.issuerInfo( QSslCertificate::OrganizationalUnitName );
    qDebug() << "StateOrProvinceName:\t" << cert.issuerInfo( QSslCertificate::StateOrProvinceName );

    qDebug() << "\n== Certificate ==";
    qDebug() << "Serial Number:\t\t" << cert.serialNumber();
    qDebug() << "Effective Date:\t\t" << cert.effectiveDate().toString();
    qDebug() << "Expiry Date:\t\t" << cert.expiryDate().toString();
    //qDebug() << "Valid:\t\t\t" << (cert.isValid() ? "Yes" : "No");
}

void Connector::dumpCipher( const QSslCipher &cipher )
{
    qDebug() << "\n== Cipher ==";

    qDebug() << "Authentication:\t\t" << cipher.authenticationMethod();
    qDebug() << "Encryption:\t\t" << cipher.encryptionMethod();
    qDebug() << "Key Exchange:\t\t" << cipher.keyExchangeMethod();
    if ( cipher.keyExchangeMethod() == QLatin1String("DH")
         || cipher.keyExchangeMethod() == QLatin1String("ECDH") ) {
        qDebug() << "Forward Secrecy:\t Yes";
    }
    else {
        qDebug() << "Forward Secrecy:\t No";
    }

    qDebug() << "Cipher Name:\t\t" << cipher.name();
    qDebug() << "Protocol:\t\t" <<  cipher.protocolString();
    qDebug() << "Supported Bits:\t\t" << cipher.supportedBits();
    qDebug() << "Used Bits:\t\t" << cipher.usedBits();
}

void Connector::ready()
{
    qDebug() << " === Peer Certificate ===";

    QSslCertificate cert = d->sock->peerCertificate();
    dumpCertificate( cert );

    QSslCipher cipher = d->sock->sessionCipher();
    dumpCipher( cipher );

    qDebug() << "Done";
    qApp->quit();
}

void Connector::socketError( QAbstractSocket::SocketError error )
{
    qDebug() << "Socket Error: " << d->sock->errorString() << " (" << int(error) << ")";
    qDebug() << "Connection Failed";
    qApp->quit();
}

void Connector::sslError( const QList<QSslError> &errors )
{
    foreach( const QSslError &error, errors ) {
        qDebug() << "SSL Error: " << error.errorString();
        QSslCertificate cert = error.certificate();
        if (cert.isNull()) {
            qDebug() << "\tCertificate: <null>";
        }
        else {
            qDebug() << "\tCommon Name: " << cert.subjectInfo(QSslCertificate::CommonName);
            qDebug() << "\tOrganization: " << cert.subjectInfo(QSslCertificate::Organization);
        }            
    }

    // This is only used because we are interested in dumping all the info
    // do NOT use if you want real security (it introduces man-in-the-middle
    // attacks).
    d->sock->ignoreSslErrors();
}
