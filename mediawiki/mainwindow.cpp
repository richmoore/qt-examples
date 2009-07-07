#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDebug>

#include "mainwindow.h"
#include "mediawiki.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    edit = new QLineEdit( this );
    list = new QListWidget( this );
    layout = new QVBoxLayout( this );
    layout->addWidget( edit );
    layout->addWidget( list );

    connect( edit, SIGNAL(returnPressed()), SLOT(go()) );

    wiki = new MediaWiki( this );
    wiki->setMaxItems(20);
    //wiki->setApiUrl( QUrl("http://techbase.kde.org/api.php") );

    connect( wiki, SIGNAL(finished(bool)), SLOT(finished(bool)) );
}

MainWindow::~MainWindow()
{
}

void MainWindow::go()
{
    wiki->search( edit->text() );
}

void MainWindow::finished( bool ok )
{
    list->clear();

    MediaWiki::Result res;
    foreach( res, wiki->results() ) {
	//qDebug() << res.title;
	list->addItem( res.title );
    }
}
