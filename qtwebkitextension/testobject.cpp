#include <qdebug.h>
#include <qwebview.h>
#include <qwebframe.h>

#include "testobject.h"

MyApi::MyApi( QObject *parent )
    : QObject( parent )
{

}

void MyApi::setWebView( QWebView *view )
{
    QWebPage *page = view->page();
    frame = page->mainFrame();

    attachObject();
    connect( frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()) );
}

void MyApi::attachObject()
{
    frame->addToJavaScriptWindowObject( QString("MyApi"), this );
}

void MyApi::doSomething( const QString &param )
{
    qDebug() << "doSomething called with parameter " << param;
}

int MyApi::doSums( int a, int b )
{
    return a + b;
}
