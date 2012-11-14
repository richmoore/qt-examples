#include <QWebView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QMessageBox>
#include <QMap>
#include <QByteArray>
#include <QDebug>

#include "mainview.h"

struct MainViewPrivate
{
    QLineEdit *url;
    QWebView *view;
    QPlainTextEdit *errors;
    QMap<QPair<QString,int>, QByteArray> exceptions;
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

    // Check for existing exception
    QUrl url = reply->request().url();

    if (d->exceptions.contains( QPair<QString, int>(url.host(), url.port()) )) {

        // Get the certificate digest
        QSslConfiguration sslConfig = reply->sslConfiguration();
        QSslCertificate peerCert = sslConfig.peerCertificate();
        QByteArray digest = peerCert.digest();

        // If the digest of the stored exception matches the one in use
        QByteArray storedDigest = d->exceptions.value(QPair<QString, int>(url.host(), url.port()));

        if (storedDigest == digest ) {
            reply->ignoreSslErrors();
            return;
        }
    }


    QMessageBox::StandardButton result = QMessageBox::question(this,
                                                               QString("Accept this certificate?"),
                                                               QString("Do you want to ignore the certificate problems?"),
                                                               QMessageBox::Yes | QMessageBox::No,
                                                               QMessageBox::No);
    // If the user wants to skip the error
    if (result == QMessageBox::Yes) {
        // Get the certificate digest
        QSslConfiguration sslConfig = reply->sslConfiguration();
        QSslCertificate peerCert = sslConfig.peerCertificate();
        QByteArray digest = peerCert.digest();

        d->exceptions.insert( QPair<QString, int>(url.host(), url.port()), digest );

        reply->ignoreSslErrors();
    }
}

