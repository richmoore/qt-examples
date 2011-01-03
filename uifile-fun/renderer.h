#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>

class QWidget;

class Renderer : public QObject
{
    Q_OBJECT
public:
    Renderer(QObject *parent = 0);

    void setTarget( QWidget *target );
    void setSvgFile( const QString &filename );
    void setPdfFile( const QString &filename );

    bool renderToSvg();
    bool renderToPdf();

public slots:
    void start();

private:
    QWidget *target;
    QString svgFile;
    QString pdfFile;
};

#endif // RENDERER_H
