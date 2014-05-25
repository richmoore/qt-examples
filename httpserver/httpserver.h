#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>

class QTcpServer;

class HttpServer : public QObject
{
    Q_OBJECT
public:
    HttpServer(quint16 port, QObject *parent=0);
    ~HttpServer();

protected slots:
    void handleConnection();

private:
    QTcpServer *server;
};

#endif // HTTPSERVER_H
