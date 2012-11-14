#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QList>

class QNetworkReply;
class QSslError;

class MainView : public QWidget
{
    Q_OBJECT
public:
    MainView(QWidget *parent=0);
    ~MainView();

public slots:
    void visitPage();
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    struct MainViewPrivate *d;
};

#endif // MAINVIEW_H
