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
    void certificateWarning(const QSslSocket *socket, const QString &message);

private:
    bool hasCertificateCache() const;
    bool createCertificateCache();

    QSslCertificate cachedCertificate(const QString &peerName);
    void addCertificate(const QString &peerName, const QSslCertificate &cert);

    QString evaluateCertificateChange( const QString &peerName, const QSslCertificate &cached,
                                       const QSslCertificate &actual, bool trusted );

private slots:
    void socketReady(QObject *sock);

private:
    class SslCertificateMonitorPrivate *d;
};

#endif // SSLCERTIFICATEMONITOR_H

