#include <qapplication.h>

#include "toplevel.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    TopLevel tl;
    tl.show();
    app.exec();
};
