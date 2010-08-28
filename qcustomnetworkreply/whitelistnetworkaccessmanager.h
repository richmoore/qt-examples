#ifndef WHITELISTNETWORKACCESSMANAGER_H
#define WHITELISTNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QMap>

class WhiteListNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit WhiteListNetworkAccessManager( QObject *parent = 0 );

    bool isAllowed( const QString &domain );

public slots:
    void addDomain( const QString &domain );

protected:
    QNetworkReply *createRequest( Operation op,
                                  const QNetworkRequest &req,
                                  QIODevice *outgoingData );

private:
    QMap<QString, int> allowedDomains;
};

#endif // WHITELISTNETWORKACCESSMANAGER_H
