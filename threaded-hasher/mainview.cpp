#include <QDebug>

#include "hasherthread.h"
#include "mainview.h"
#include "ui_mainview.h"

MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);

    hasher = new HasherThread( this );
    connect( hasher, SIGNAL(error(const QString &)), ui->resultEdit, SLOT(setText(const QString &)) );
    connect( hasher, SIGNAL(completed(const QString &)), ui->resultEdit, SLOT(setText(const QString &)) );

    connect( ui->hashButton, SIGNAL(clicked()), SLOT(hashPressed()) );
}

MainView::~MainView()
{
    delete ui;
}


void MainView::hashPressed()
{
    if ( ui->filenameEdit->text().isEmpty() )
        return;
    hasher->setFilename( ui->filenameEdit->text() );

    switch( ui->algorithmCombo->currentIndex() ) {
    case 0:
        hasher->setAlgorithm( QCryptographicHash::Sha1 );
        break;
    case 1:
        hasher->setAlgorithm( QCryptographicHash::Md5 );
        break;
    case 2:
        hasher->setAlgorithm( QCryptographicHash::Md4 );
        break;
    }

    ui->resultEdit->setText( QString("Working...") );
    hasher->start();
}
