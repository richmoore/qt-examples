
function test( actual, expected ) {
    if ( actual == expected ) {
	debug( "PASS: " + actual );
    }
    else {
	debug( "FAIL: " + actual + " should be " + expected );
    }
};

debug( 'Starting tests' );

debug( myIpAddress() );

b = isInNet( "192.168.1.1", "192.168.1.0", "255.255.255.0" );
test( b, true );

b = isInNet( "192.168.2.1", "192.168.1.0", "255.255.255.0" );
test( b, false );

b = isInNet( "192.168.2.1", "192.168.1.0", "255.255.0.0" );
test( b, true );

b = shExpMatch( "aaaa", "a*" );
test( b, true );

b = shExpMatch( "aaaa", "b*" );
test( b, false );

b = shExpMatch( "aaaa", "*b*" );
test( b, false );

b = shExpMatch( "aaaa", "*a*" );
test( b, true );

b = shExpMatch( "aaaa", "b*a*" );
test( b, false );

addr = dnsResolve( "needcoffee.co.uk" );
test( addr, '80.68.89.8' );

addr = dnsResolve( "www.kde.org" );
test( addr, '62.70.27.118' );

addr = dnsResolve( "doesnotexists" );
test( addr, null );

function FindProxyForURL( url, host )
{
    return "PROXY bart:8080";
}

proxy = FindProxyForURL( 'http://www.kde.org/', 'www.kde.org' );
test( proxy, 'PROXY bart:8080' );

debug( 'Done tests' );
