#include <qdebug.h>
#include <qstringlist.h>
#include <qurl.h>
#include <qlcdnumber.h>


#include "pluginfactory.h"

PluginFactory::PluginFactory( QObject *parent )
    : QWebPluginFactory( parent )
{

}

QObject *PluginFactory::create( const QString &mimeType, const QUrl &url,
				const QStringList &argumentNames, const QStringList &argumentValues ) const
{
    qDebug() << "create" << mimeType << "for url" << url;
    QLCDNumber *lcd = new QLCDNumber();
    lcd->display(123);

    return lcd;
}

QList<QWebPluginFactory::Plugin> PluginFactory::plugins() const
{
    qDebug() << "plugins called";

    QWebPluginFactory::MimeType mime;
    mime.description = QString("My mime");
    mime.name =  QString("rich/test");

    QWebPluginFactory::Plugin plug;
    plug.description = QString("A test plugin");
    plug.mimeTypes += mime;

    QList<QWebPluginFactory::Plugin> plugins;
    plugins += plug;

    return plugins;
}
