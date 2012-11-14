#include <QWebView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QSslSocket>
#include <QSslCertificate>
#include <QFile>

#include "mainview.h"

struct MainViewPrivate
{
    QLineEdit *url;
    QWebView *view;
    QPlainTextEdit *errors;
};

MainView::MainView(QWidget *parent)
    : QWidget(parent),
      d(new MainViewPrivate)
{
    d->url = new QLineEdit(this);
    d->view = new QWebView(this);
    d->errors = new QPlainTextEdit(this);
    
    // Make errors bigger
    QFont f(d->errors->font());
    f.setPointSize(18);
    d->errors->setFont(f);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(d->url);
    layout->addWidget(d->view, 1);
    layout->addWidget(d->errors, 1);

    connect(d->url, SIGNAL(returnPressed()), this, SLOT(visitPage()));

    // Setup QNAM
    QNetworkAccessManager *nam = d->view->page()->networkAccessManager();
    connect(nam, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(finished(QNetworkReply *)));

    connect(nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
            this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));

    // Add custom CA
    QFile cafile("cacert.pem");
    cafile.open(QIODevice::ReadOnly);

    QSslCertificate cert(&cafile);
    QSslSocket::addDefaultCaCertificate(cert);
}

MainView::~MainView()
{
    delete d;
}

void MainView::visitPage()
{
    d->errors->clear();
    d->view->load(d->url->text());
}

void MainView::finished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        d->errors->appendPlainText( QString("Network Error: %1").arg(reply->errorString()) );
    }
}

void MainView::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    foreach( const QSslError &error, errors ) {
        d->errors->appendPlainText( QString("SSL Error: %1").arg(error.errorString()) );
    }
}
