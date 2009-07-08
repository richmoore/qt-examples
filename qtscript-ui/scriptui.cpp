#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QRegExp>
#include <QtGui/QWidget>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QtUiTools/QUiLoader>

#include "scriptui.h"

ScriptUi::ScriptUi( QObject *parent )
    : QObject( parent )
{
    engine = new QScriptEngine( this );
}

bool ScriptUi::loadScript( const QString &filename )
{
    QFile f( filename );
    if ( !f.open( QFile::ReadOnly ) )
	return false;

    QTextStream ts( &f );
    QString source =  ts.readAll();
    f.close();

    QScriptValue r = engine->evaluate(source);
    if ( engine->hasUncaughtException() ) {
	qDebug() << "Uncaught js exception,"  << r.toString() 
		 << "at line" << engine->uncaughtExceptionLineNumber();

	return false;
    }

    return true;
}

bool ScriptUi::loadUi( const QString &filename )
{
    QUiLoader loader;
    QFile f( filename );
    if ( !f.open(QIODevice::ReadOnly) )
	return false;
    ui = loader.load( &f );
    if ( !ui )
	return false;

    return true;
}

/*
 * Connect functions with names of the form on_<objectname>_<signalname> to
 * the signal.
 */
void ScriptUi::autoConnectFunctions()
{
    QScriptValue global = engine->globalObject();

    QScriptValueIterator it( global );
    while ( it.hasNext() ) {
	it.next();

	// We only care about functions
	if ( !it.value().isFunction() )
	    continue;
	// and they start with on_
	if ( !it.name().startsWith( QString("on_") ) )
	    continue;
	
	// extract the object and signal names we can't use \w in this regexp
	// as we're using _ as a separator.
	//QRegExp re( "on_([\\da-zA-Z])+_([\\da-zA-Z])+" ); // QRegExp bug (quantifier is invalid)
	QRegExp re( "on_([\\da-zA-Z]+)_([\\da-zA-Z]+)" );
	if ( !re.exactMatch( it.name() ) )
	    continue;

	QString objectName = re.cap(1);
	QString signalName = re.cap(2);

	qDebug() << "found on_ object " << objectName << " signal is " << signalName;
	QWidget  *obj = ui->findChild<QWidget *>( objectName );
	if ( !obj ) {
	    qDebug() << "Warning: Unable to find child named " << objectName;
	    continue;
	}

	// TODO: Support signals with arguments
	signalName = QString("2%1()").arg(signalName); // Evil hack
	QByteArray sName = signalName.toUtf8();
	bool ok = qScriptConnect( obj, sName.data(), global, it.value() );
	qDebug() << "Connect result is " << ok;
    }
}

void ScriptUi::exposeWidgets()
{
    QScriptValue global = engine->globalObject();

    QList<QWidget *> kids = ui->findChildren<QWidget *>();
    foreach ( QWidget *child, kids ) {
	QScriptValue wrapper = engine->newQObject( child );
	global.setProperty( child->objectName(), wrapper );
    }
}

void ScriptUi::connectScript()
{
    // autoconnect all methods from the script to the appropriate widgets
    autoConnectFunctions();

    // Expose all widgets from the ui file to the script
    exposeWidgets();
}

QWidget *ScriptUi::view() const
{
    return ui;
}
