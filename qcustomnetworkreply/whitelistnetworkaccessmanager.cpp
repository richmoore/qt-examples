#include <QNetworkRequest>
#include <QDebug>

#include "qcustomnetworkreply.h"
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
    return allowedDomains.contains(domain);
}

QNetworkReply *WhiteListNetworkAccessManager::createRequest( Operation op,
                                                             const QNetworkRequest &req,
                                                             QIODevice *outgoingData )
{
    QNetworkRequest myReq( req );

    // If host is not whitelisted then kill it
    if ( !isAllowed( req.url().host() ) ) {
        QCustomNetworkReply *reply = new QCustomNetworkReply();
        reply->setHttpStatusCode( 403, "Forbidden" );
        reply->setContentType("text/html");
        reply->setContent( QString("<html><body><h1>That url is not in the whitelist</h1></body></html>") );

        return reply;
    }

    QNetworkReply *reply = QNetworkAccessManager::createRequest( op, myReq, outgoingData );
    return reply;
}
