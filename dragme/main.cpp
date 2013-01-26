#include <QApplication>
#include "draggable.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Draggable d;
    d.show();

    return app.exec();
}
