#ifndef SCRIPTUI_H
#define SCRIPTUI_H

#include <qobject.h>

class QWidget;
class QScriptEngine;

class ScriptUi : public QObject
{
    Q_OBJECT
public:
    ScriptUi( QObject *parent = 0 );

    bool loadScript( const QString &filename );
    bool loadUi( const QString &filename );

    void connectScript();

    QWidget *view() const;

private:
    void autoConnectFunctions();


private:
    QScriptEngine *engine;
    QWidget *ui;
};

#endif // SCRIPTUI_H

