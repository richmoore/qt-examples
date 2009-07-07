#include <qregexp.h>
#include <stdio.h>

/**
 * This regexp is invalid as the quantifier should only work inside
 * the group. Instead the regexp is treated as valid.
 */
int main( int argc, char **argv )
{
    // QRegExp re( "on_([\\da-zA-Z])+_([\\da-zA-Z])+" ); // QRegExp bug (quantifier is invalid)
    QRegExp broken( "(test)+" );
    printf( "isValid=%d\n", broken.isValid() );
}
