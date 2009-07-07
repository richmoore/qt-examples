#include <qregexp.h>
#include <stdio.h>

int main( int argc, char **argv )
{
    // QRegExp re( "on_([\\da-zA-Z])+_([\\da-zA-Z])+" ); // QRegExp bug (quantifier is invalid)
    QRegExp broken( "(test)+" );
    printf( "isValid=%d\n", broken.isValid() );
}
