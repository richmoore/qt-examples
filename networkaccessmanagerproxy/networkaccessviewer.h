#ifndef NETWORKACCESSVIEWER_H
#define NETWORKACCESSVIEWER_H

#include <qobject.h>
#include <qmap.h>
#include <qpair.h>
#include <qlist.h>
#include <qbytearray.h>
#include <qnetworkaccessmanager.h>

#include "ui_networkrequests.h"

class QDialog;
class QTreeWidgetItem;
class QSignalMapper;

class NetworkAccessViewer : public QObject
{
    Q_OBJECT

public:
    NetworkAccessViewer();
    ~NetworkAccessViewer();

    void addRequest( QNetworkAccessManager::Operation op, const QNetworkRequest&req, QIODevice *outgoingData, QNetworkReply *reply );

    void show();
    void hide();

private slots:
    void clear();
    void requestFinished( QObject *replyObject );
    void showItemDetails( QTreeWidgetItem *item );

private:
    QDialog *dialog;
    Ui::NetworkRequestsDialog *networkRequestsDialog;
    QMap<QNetworkReply *, QNetworkRequest> requestMap;
    QMap<QTreeWidgetItem *, QNetworkRequest> itemRequestMap;
    QMap<QNetworkReply *, QTreeWidgetItem *> itemMap;
    QMap<QTreeWidgetItem *, QPair< QList<QByteArray>, QList<QByteArray> > > itemReplyMap;
    QSignalMapper *mapper;
};

#endif // NETWORKACCESSVIEWER_H
