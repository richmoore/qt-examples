#ifndef HASHERTHREAD_H
#define HASHERTHREAD_H

#include <QThread>
#include <QCryptographicHash>

class HasherThread : public QThread
{
Q_OBJECT
public:
    explicit HasherThread(QObject *parent = 0);
    ~HasherThread();

    void setFilename( const QString &filename );
    void setAlgorithm( QCryptographicHash::Algorithm alg );

signals:
    void error( const QString &message );
    void completed( const QString &hash );

protected:
    void run();

private:
    QCryptographicHash *hasher;
    QString filename;
};

#endif // HASHERTHREAD_H
