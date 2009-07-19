#ifndef KWEBTHUMBNAILER_H
#define KWEBTHUMBNAILER_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QImage>

class QUrl;
class QSize;

class KWebThumbnailer : public QObject
{
    Q_OBJECT

public:
    KWebThumbnailer( QObject *parent = 0 );
    KWebThumbnailer( const QUrl &url, const QSize &size, QObject *parent = 0 );
    ~KWebThumbnailer();

    void setUrl( const QUrl &url );
    void setSize( const QSize &size );
    void setZoomFactory( double zoom );

    void start();

    QPixmap thumbnail() const;

    bool isValid() const;

signals:
    void done( bool success );

private slots:
    void completed( bool success );

private:
    class KWebThumbnailerPrivate *d;
};

#endif // KWEBTHUMBNAILER_H

