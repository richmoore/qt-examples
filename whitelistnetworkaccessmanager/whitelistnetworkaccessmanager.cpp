#include <QNetworkRequest>

#include "whitelistnetworkaccessmanager.h"

WhiteListNetworkAccessManager::WhiteListNetworkAccessManager( QObject *parent ) :
    QNetworkAccessManager( parent )
{
}

void WhiteListNetworkAccessManager::addDomain( const QString &domain )
{
    allowedDomains.insert( domain, 1 );
}

bool WhiteListNetworkAccessManager::isAllowed( const QString &domain )
{
    return (allowedDomains[domain] == 1); // Rely on Qt's default of 0 for non-existent ints
}

QNetworkReply *WhiteListNetworkAccessManager::createRequest( Operation op,
                                                             const QNetworkRequest &req,
                                                             QIODevice *outgoingData )
{
    QNetworkRequest myReq( req );

    // If host is not whitelisted then kill it
    if ( !isAllowed( req.url().host() ) ) {
        myReq.setUrl( QUrl( QString("forbidden://localhost/") ) );
    }

    QNetworkReply *reply = QNetworkAccessManager::createRequest( op, myReq, outgoingData );
    return reply;
}
