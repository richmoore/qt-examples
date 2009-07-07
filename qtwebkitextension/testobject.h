#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <qobject.h>

class QWebView;
class QWebFrame;

class MyApi : public QObject
{
    Q_OBJECT
public:
    MyApi( QObject *parent );

    void setWebView( QWebView *view );

public slots:
    void doSomething( const QString &param );
    int doSums( int a, int b );

private slots:
    void attachObject();

private:
    QWebFrame *frame;
};

#endif // TESTOBJECT_H

