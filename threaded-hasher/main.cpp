#include <QtGui/QApplication>
#include "mainview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainView w;
    w.show();

    return a.exec();
}
