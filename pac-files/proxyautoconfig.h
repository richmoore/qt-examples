// -*- c++ -*-

#ifndef PROXYAUTOCONFIG_H
#define PROXYAUTOCONFIG_H

#include <qobject.h>
#include <qscriptvalue.h>

class QScriptContext;
class QScriptEngine;

/**
 * Class implementing the proxy auto-configuration (PAC) JavaScript api.
 */
class ProxyAutoConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString config WRITE setConfig )

public:
    ProxyAutoConfig();
    ~ProxyAutoConfig();

    /**
     * Call this to set the script to be executed. Note that the argument should be
     * the content of the .pac file to be used, not the URL where it is located.
     */
    void setConfig( const QString &config );

    /**
     * Returns the result 
     */
    Q_SCRIPTABLE QString findProxyForUrl( const QString &url, const QString &host );

private:
    void install();

    static QScriptValue debug( QScriptContext *context, QScriptEngine *engine );

    /* String myIpAddress */
    static QScriptValue myIpAddress( QScriptContext *context, QScriptEngine *engine );

    /* bool isInNet ipaddress, netaddress, netmask */
    static QScriptValue isInNet( QScriptContext *context, QScriptEngine *engine );

    /* bool shExpMatch url, glob */
    static QScriptValue shExpMatch( QScriptContext *context, QScriptEngine *engine );

    /* string dnsResolve hostname */
    static QScriptValue dnsResolve( QScriptContext *context, QScriptEngine *engine );

private:
    QScriptEngine *engine;
};

#endif // PROXYAUTOCONFIG_H
