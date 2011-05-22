#include <QApplication>
#include <QDebug>

#include "monitortest.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

    MonitorTest test;
    test.show();

    return app.exec();
}
