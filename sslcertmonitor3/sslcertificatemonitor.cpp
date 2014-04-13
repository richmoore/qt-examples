/**
 * (c) 2011 Richard Moore, rich@kde.org
 */

#include <QSignalMapper>
#include <QSslCertificate>
#include <QSslSocket>
#include <QFile>
#include <QDir>
#include <QCache>

#include "sslcertificatemonitor.h"

/*
 * Limitations:
 *
 * - Cannot be extended to QNAM right now as that neither gives us access
 * to the underlying QSslSocket not provides a function for encrypted() that
 * is analogous to the sslError() forwarding function.
 *
 * - Since we need to tap the encrypted() signal, it's not safe for the
 * application to send data in response to the signal. We really need to
 * have a signal that works immediately prior to the one the application
 * uses, or get the application to use a signal sent by this class.
 *
 * - Doesn't track the date or anything from the cert (could easily be
 * added eg. by using the modification time on the cache).
 *
 * - There is no API for simply checking if a cert is signed by a trusted
 * CA. This means we can't trivially spot the case where a previously
 * good cert is now signed by an untrusted CA.
 */

class SslCertificateMonitorPrivate
{
public:
    QString cacheDir;
    bool acceptCurrent;
    QSignalMapper *mapper;
    QCache<QString,QSslCertificate> acceptedCache;
};

static const int ACCEPTED_CERT_CACHE_SIZE = 20;

SslCertificateMonitor::SslCertificateMonitor(QObject *parent)
    : QObject(parent),
      d(new SslCertificateMonitorPrivate)
{
    d->cacheDir = QLatin1String(".certificate_cache");
    d->acceptCurrent = false;

    d->mapper = new QSignalMapper(this);
    connect( d->mapper, SIGNAL(mapped(QObject *)), SLOT(socketReady(QObject *)) );

    d->acceptedCache.setMaxCost(ACCEPTED_CERT_CACHE_SIZE);
}

SslCertificateMonitor::~SslCertificateMonitor()
{
    delete d;
}

void SslCertificateMonitor::addSocket(QSslSocket *socket)
{
    d->mapper->setMapping(socket, socket);
    connect(socket, SIGNAL(encrypted()), d->mapper, SLOT(map()));
}

void SslCertificateMonitor::removeSocket(QSslSocket *socket)
{
    d->mapper->removeMappings(socket);
}

void SslCertificateMonitor::acceptCertificate()
{
    d->acceptCurrent = true;
}

bool SslCertificateMonitor::hasCertificateCache() const
{
    QDir cache(d->cacheDir);
    return cache.exists();
}

bool SslCertificateMonitor::createCertificateCache()
{
    QDir cache = QDir::current();
    return cache.mkdir(d->cacheDir);
}

void SslCertificateMonitor::addCertificate(const QString &peerName, const QSslCertificate &cert)
{
    if (!hasCertificateCache()) {
        bool ok = createCertificateCache();
        if (!ok)
            return;
    }

    QString cacheEntry = d->cacheDir + QLatin1Char('/') + peerName;
    QFile f( cacheEntry );
    if (!f.open(QIODevice::WriteOnly))
        return;

    // TODO: write the whole chain
    f.write(cert.toPem());
    f.close();
}

QSslCertificate SslCertificateMonitor::cachedCertificate(const QString &peerName)
{
    QString cacheEntry = d->cacheDir + QLatin1Char('/') + peerName;
    QFile f( cacheEntry );
    if (!f.open(QIODevice::ReadOnly))
        return QSslCertificate();

    QSslCertificate cert(&f);
    f.close();
    
    return cert;
}

QString SslCertificateMonitor::evaluateCertificateChange( const QString &peerName,
                                                          const QSslCertificate &cached,
                                                          const QSslCertificate &actual,
                                                          bool trusted )
{
    // BEWARE: check if the past cert had ssl errors or someone can just spoof
    // this stuff trivially. The info on the current cert is only trust-worthy if
    // trusted is true.

    bool cachedCertIsExpired = (cached.expiryDate() < QDateTime::currentDateTime());

    bool issuerIsSame = false;
    if ( cached.issuerInfo(QSslCertificate::Organization) == actual.issuerInfo(QSslCertificate::Organization)
         && cached.issuerInfo(QSslCertificate::CommonName) == actual.issuerInfo(QSslCertificate::CommonName)
         && cached.issuerInfo(QSslCertificate::LocalityName) == actual.issuerInfo(QSslCertificate::LocalityName)
         && cached.issuerInfo(QSslCertificate::OrganizationalUnitName) == actual.issuerInfo(QSslCertificate::OrganizationalUnitName)
         && cached.issuerInfo(QSslCertificate::CountryName) == actual.issuerInfo(QSslCertificate::CountryName)
         && cached.issuerInfo(QSslCertificate::StateOrProvinceName) == actual.issuerInfo(QSslCertificate::StateOrProvinceName) )
        {
            issuerIsSame = true;
        }

    bool cachedWasSelfSigned = false;
    if ( cached.issuerInfo(QSslCertificate::Organization) == cached.subjectInfo(QSslCertificate::Organization)
         && cached.issuerInfo(QSslCertificate::CommonName) == cached.subjectInfo(QSslCertificate::CommonName)
         && cached.issuerInfo(QSslCertificate::LocalityName) == cached.subjectInfo(QSslCertificate::LocalityName)
         && cached.issuerInfo(QSslCertificate::OrganizationalUnitName) == cached.subjectInfo(QSslCertificate::OrganizationalUnitName)
         && cached.issuerInfo(QSslCertificate::CountryName) == cached.subjectInfo(QSslCertificate::CountryName)
         && cached.issuerInfo(QSslCertificate::StateOrProvinceName) == cached.subjectInfo(QSslCertificate::StateOrProvinceName) )
        {
            cachedWasSelfSigned = true;
        }

    bool actualIsSelfSigned = false;
    if ( actual.issuerInfo(QSslCertificate::Organization) == actual.subjectInfo(QSslCertificate::Organization)
         && actual.issuerInfo(QSslCertificate::CommonName) == actual.subjectInfo(QSslCertificate::CommonName)
         && actual.issuerInfo(QSslCertificate::LocalityName) == actual.subjectInfo(QSslCertificate::LocalityName)
         && actual.issuerInfo(QSslCertificate::OrganizationalUnitName) == actual.subjectInfo(QSslCertificate::OrganizationalUnitName)
         && actual.issuerInfo(QSslCertificate::CountryName) == actual.subjectInfo(QSslCertificate::CountryName)
         && actual.issuerInfo(QSslCertificate::StateOrProvinceName) == actual.subjectInfo(QSslCertificate::StateOrProvinceName) )
        {
            actualIsSelfSigned = true;
        }

    /**
     * We have the following metrics with which to evaluate the certificate:
     * cachedCertIsExpired
     * issuerIsSame
     * cachedWasSelfSigned
     * actualIsSelfSigned
     * trusted
     *
     * Note that just because the certs are not self-signed doesn't mean the
     * CA that signed them is trusted, ideally we would be able to verify the
     * chain directly from the cert but the API does not allow this. For now,
     * we check if the current cert had any SSL errors by passing it in via
     * the 'trusted' variable.
     */

    if ( cachedCertIsExpired && issuerIsSame && trusted ) {
        QString message = tr("This is probably fine, but the certificate for %1 has changed " \
                             "since you previously visited. The old certificate has now " \
                             "expired and the new one is valid and from the same issuer.");

        message = message.arg(peerName);
        return message;
    }
    else if ( cachedCertIsExpired && trusted ) {
        // issuerIsSame == false
        QString message = tr("This is probably fine, but the certificate for %1 has changed " \
                             "since you previously visited. The old certificate has now " \
                             "expired and the new one is valid. T new certificate is from " \
                             "a different issuer however.");

        message = message.arg(peerName);
        return message;
    }
    else if ( !cachedCertIsExpired && issuerIsSame && trusted ) {
        QString message = tr("This is a bit suspicious, the certificate for %1 has changed " \
                             "since you previously visited, even though the old certificate " \
                             "would still be fine. The new one is valid and from the same "
                             "issuer.");

        message = message.arg(peerName);
        return message;

    }
    else if ( !cachedCertIsExpired && !issuerIsSame && cachedWasSelfSigned && trusted ) {
        QString message = tr("This is probably a good thing, but the certificate for %1 " \
                             "has changed since you previously visited. The old certificate " \
                             "was self-signed, and the new one has been issued by a trusted " \
                             "authority.");

        message = message.arg(peerName);
        return message;

    }
    else if ( !cachedCertIsExpired && !issuerIsSame && cachedWasSelfSigned && actualIsSelfSigned ) {
        QString message = tr("This is suspicious, the certificate for %1 has changed since you " \
                             "previously visited. The old certificate was self-signed and has not " \
                             "expired yet, the new one is self-signed as well, so we can't verify it.");

        message = message.arg(peerName);
        return message;
    }
    else if ( !cachedCertIsExpired && !issuerIsSame && !cachedWasSelfSigned && actualIsSelfSigned ) {
        QString message = tr("This is highly suspicious, you should not trust this site at the moment. " \
                             "The certificate for %1 has changed since you previously visited, the old " \
                             "certificate was not self-signed and has not expired yet, but the new one " \
                             "is self-signed.");

        message = message.arg(peerName);
        return message;
    }

}

void SslCertificateMonitor::socketReady(QObject *sockobj)
{
    QSslSocket *sock = qobject_cast<QSslSocket *>(sockobj);
    if (!sock)
        return;

    QString peerName = sock->peerName();
    QSslCertificate certificate = sock->peerCertificate();

    if (*(d->acceptedCache.object(peerName)) == certificate)
        return; // Fast path for most recently accepted certificates

    // Have we been here before?
    QSslCertificate previousCertificate = cachedCertificate(peerName);

    if (!previousCertificate.isNull()) {
        if (certificate == previousCertificate) {
            // We need to add the certificate to the cache here as well as when we add to
            // the on-disk cache so that we don't hit the disk again for this site.
            d->acceptedCache.insert(peerName,new QSslCertificate(certificate));
            return; // All is well
        }

        // Cert has changed
        QString message = evaluateCertificateChange( peerName, previousCertificate,
                                                     certificate, sock->sslErrors().isEmpty() );

        d->acceptCurrent = false;
        emit certificateWarning(sock, message);
    }
    else {
        // The certificate is new. We don't show anything to user because then
        // we're simply training them to click through our warning message without
        // thinking.
        d->acceptCurrent = true;
    }

    // If the user has chosen to accept the certificate or the certficate is new
    // then we store the updated entry.
    if (d->acceptCurrent) {
        d->acceptedCache.insert(peerName,new QSslCertificate(certificate));
        addCertificate(peerName, certificate);
    }
    else {
        // Certficate has been considered dangerous by the user
        sock->abort();
    }
}
