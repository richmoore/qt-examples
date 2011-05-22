// -*- c++ -*-

/**
 * (c) 2011 Richard Moore, rich@kde.org
 */

#ifndef SSLCERTIFICATEMONITOR_H
#define SSLCERTIFICATEMONITOR_H

#include <QObject>

class QSslSocket;
class QSslCertificate;

class SslCertificateMonitor : public QObject
{
    Q_OBJECT
public:
    SslCertificateMonitor(QObject *parent=0);
    ~SslCertificateMonitor();

    void addSocket(QSslSocket *socket);
    void removeSocket(QSslSocket *socket);

public slots:
    void acceptCertificate();

signals:
    void certificateWarning(const QString &peerName, const QString &message);

private:
    bool hasCertificateCache() const;
    bool createCertificateCache();

    bool hasCachedCertificate(const QString &peerName);
    void addCertificate(const QString &peerName, const QSslCertificate &cert);
    bool isMatchingCertificate(const QString &peerName, const QSslCertificate &cert);

private slots:
    void socketReady(QObject *sock);

private:
    class SslCertificateMonitorPrivate *d;
};

#endif // SSLCERTIFICATEMONITOR_H

