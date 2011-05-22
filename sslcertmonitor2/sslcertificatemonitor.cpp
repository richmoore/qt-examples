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
 */

class SslCertificateMonitorPrivate
{
public:
    QString cacheDir;
    bool acceptCurrent;
    QSignalMapper *mapper;
    QCache<QString,QByteArray> acceptedCache;
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
    QByteArray digest = cert.digest();
    d->acceptedCache.insert(peerName,new QByteArray(digest));

    if (!hasCertificateCache()) {
        bool ok = createCertificateCache();
        if (!ok)
            return;
    }

    QString cacheEntry = d->cacheDir + QLatin1Char('/') + peerName;
    QFile f( cacheEntry );
    if (!f.open(QIODevice::WriteOnly))
        return;

    f.write(digest);
    f.close();
}

bool SslCertificateMonitor::hasCachedCertificate(const QString &peerName)
{
    QString cacheEntry = d->cacheDir + QLatin1Char('/') + peerName;
    QFile f( cacheEntry );
    return f.exists();
}

bool SslCertificateMonitor::isMatchingCertificate(const QString &peerName, const QSslCertificate &cert)
{
    QString cacheEntry = d->cacheDir + QLatin1Char('/') + peerName;
    QFile f( cacheEntry );
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QByteArray oldDigest = f.readAll();
    f.close();

    if (oldDigest != cert.digest())
        return false;

    return true;
}

void SslCertificateMonitor::socketReady(QObject *sockobj)
{
    QSslSocket *sock = qobject_cast<QSslSocket *>(sockobj);
    if (!sock)
        return;

    QString peerName = sock->peerName();
    QSslCertificate certificate = sock->peerCertificate();
    QByteArray digest = certificate.digest();

    if (d->acceptedCache.object(peerName) == digest)
        return; // Fast path for most recently accepted certificate

    // Have we been here before?
    if (hasCachedCertificate(peerName)) {
        if (isMatchingCertificate(peerName, certificate)) {
            // We need to add the digest to the cache here as well as when we add to
            // the on-disk cache so that we don't hit the disk again for this site.
            d->acceptedCache.insert(peerName,new QByteArray(digest));
            return; // All is well
        }

        // Cert has changed
        QString message = tr("The certificate for %1 has changed since you previously visited, " \
                             "it could be that someone is trying to intercept your communication.");
        message = message.arg(peerName);

        d->acceptCurrent = false;
        emit certificateWarning(peerName, message);
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
        addCertificate(peerName, certificate);
    }
    else {
        // Certficate has been considered dangerous by the user
        sock->abort();
    }
}
