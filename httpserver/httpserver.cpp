#include <QTcpServer>
#include <QTcpSocket>

#include "httprequesthandler.h"
#include "httpserver.h"

HttpServer::HttpServer(quint16 port, QObject *parent)
    : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(handleConnection()));
    server->listen(QHostAddress::Any, port);
}

HttpServer::~HttpServer()
{
}

void HttpServer::handleConnection()
{
    while(server->hasPendingConnections()) {
        QTcpSocket *sock = server->nextPendingConnection();
        HttpRequestHandler *handler = new HttpRequestHandler(sock, this);
    }
}
