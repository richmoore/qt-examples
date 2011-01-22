#ifndef RESIZER_H
#define RESIZER_H

#include <QObject>

class QImage;
class QPixmap;
class QSize;

class Resizer : public QObject
{
    Q_OBJECT
public:
    Resizer( QObject *parent=0 );
    ~Resizer();

public slots:
    void setSize( const QSize &size );
    void setAspectRatioMode( const Qt::AspectRatioMode mode );
   
    void setInput( const QImage &input );
    void setInput( const QString &filename );

    void start();

signals:
    void error();
    void finished( const QImage &output );

private:
    struct ResizerPrivate *d;
};

#endif // RESIZER_H

