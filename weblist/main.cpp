#include <QtGui>
#include <qlistwidgetng.h>
#include <qlistdefaultmodel.h>
#include <qlistcontroller.h>

#include "webview.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QtListWidgetNG widget;

    WebView view;

    widget.defaultModel()->appendItem(new QtListDefaultItem("http://www.kde.org/"));
    widget.defaultModel()->appendItem(new QtListDefaultItem("http://news.bbc.co.uk/"));

    widget.controller()->setView(&view);

    widget.resize(QSize(240, 320));
    widget.show();
    return app.exec();
}
