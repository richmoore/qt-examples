#ifndef MONITORTEST_H
#define MONITORTEST_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QTextEdit;
class QSslSocket;

class SslCertificateMonitor;

class MonitorTest : public QWidget
{
Q_OBJECT
public:
    MonitorTest();

public slots:
    void doGet();
    void certificateWarning(const QString &host, const QString &message);
    void readResponse();

private:
    QLineEdit *host;
    QPushButton *getButton;
    QTextEdit *response;
    SslCertificateMonitor *monitor;
    QSslSocket *sock;
};

#endif // MONITORTEST_H
