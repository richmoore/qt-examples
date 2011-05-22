#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSslSocket>
#include <QMessageBox>

#include "sslcertificatemonitor.h"
#include "monitortest.h"

MonitorTest::MonitorTest()
    : QWidget()
{
    monitor = new SslCertificateMonitor(this);
    connect(monitor, SIGNAL(certificateWarning(const QString &, const QString &)),
            SLOT(certificateWarning(const QString &, const QString &)));

    host = new QLineEdit;
    getButton = new QPushButton(QLatin1String("GET"));
    connect(getButton, SIGNAL(clicked()), SLOT(doGet()));
    response = new QTextEdit;

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(host);
    vbox->addWidget(getButton);
    vbox->addWidget(response);

    setLayout(vbox);
}

void MonitorTest::doGet()
{
    sock = new QSslSocket(this);
    monitor->addSocket(sock);
    connect(sock, SIGNAL(readyRead()), SLOT(readResponse()));
    sock->connectToHostEncrypted( host->text(), 443 );
    sock->write("GET / HTTP/1.0\r\n\r\n");
}

void MonitorTest::readResponse()
{
    QByteArray content = sock->readAll();
    response->setPlainText( response->toPlainText() + QString::fromLatin1(content.constData()) );
}

void MonitorTest::certificateWarning(const QString &host, const QString &message)
{
    QMessageBox::StandardButton result = 
        QMessageBox::warning( this,
                              tr("Certificate for %1 has changed").arg(host),
                              message + QLatin1String("\n\nAre you sure you wish to continue?"),
                              QMessageBox::Yes | QMessageBox::No );

    if (result == QMessageBox::Yes)
        monitor->acceptCertificate();
}

