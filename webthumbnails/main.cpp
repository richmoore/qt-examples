#include <qapplication.h>
#include "toplevel.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    TopLevel top;
    top.show();

    return app.exec();
}
