#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <QtWebKit/QWebPluginFactory>

class PluginFactory : public QWebPluginFactory
{
    Q_OBJECT
public:
    PluginFactory( QObject *parent );

    QObject *create ( const QString &mimeType, const QUrl &url,
		      const QStringList &argumentNames, const QStringList &argumentValues ) const;

    QList<QWebPluginFactory::Plugin> plugins () const;
};

#endif // PLUGINFACTORY_H
