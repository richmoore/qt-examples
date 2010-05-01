#include "networkaccessmanagerproxy.h"
#include "networkaccessviewer.h"

NetworkAccessManagerProxy::NetworkAccessManagerProxy(QObject *parent) :
    QNetworkAccessManager(parent),
    viewer(0)
{
}

void NetworkAccessManagerProxy::setNetworkAccessViewer( NetworkAccessViewer *viewer )
{
    this->viewer = viewer;
}


QNetworkReply *NetworkAccessManagerProxy::createRequest( Operation op,
                                                         const QNetworkRequest &req,
                                                         QIODevice * outgoingData )
{
    QNetworkReply *reply = QNetworkAccessManager::createRequest( op, req, outgoingData );
    viewer->addRequest( op, req, outgoingData, reply );
    return reply;
}
