#ifndef HTTPSSERVER_H
#define HTTPSSERVER_H

#include <QObject>

class QSslServer;

class HttpsServer : public QObject
{
    Q_OBJECT
public:
    HttpsServer(quint16 port, QObject *parent=0);
    ~HttpsServer();

protected slots:
    void handleConnection();

private:
    QSslServer *server;
};

#endif // HTTPSSERVER_H
