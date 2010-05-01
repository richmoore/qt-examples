#ifndef NETWORKACCESSMANAGERPROXY_H
#define NETWORKACCESSMANAGERPROXY_H

#include <QNetworkAccessManager>

class NetworkAccessViewer;

class NetworkAccessManagerProxy : public QNetworkAccessManager
{
Q_OBJECT
public:
    explicit NetworkAccessManagerProxy(QObject *parent = 0);

    void setNetworkAccessViewer( NetworkAccessViewer *viewer );

protected:
    QNetworkReply *createRequest( Operation op, const QNetworkRequest &req, QIODevice * outgoingData=0 );

private:
    NetworkAccessViewer *viewer;
};

#endif // NETWORKACCESSMANAGERPROXY_H
