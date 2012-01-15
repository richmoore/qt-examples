#ifndef GETSLASH_H
#define GETSLASH_H

#include <QObject>

class SslSocket;

class GetSlash : public QObject
{
    Q_OBJECT

public:
    GetSlash(SslSocket *sock);

public slots:
    void start();
    void gotData();

private:
    class SslSocket *sock;
};

#endif // GETSLASH_H

