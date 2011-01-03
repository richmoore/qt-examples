#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QSvgGenerator>
#include "renderer.h"

Renderer::Renderer(QObject *parent)
    : QObject(parent)
{
}

void Renderer::setTarget( QWidget *target )
{
    this->target = target;
}

void Renderer::setSvgFile( const QString &filename )
{
    svgFile = filename;
}

bool Renderer::renderToSvg()
{
    if (svgFile.isEmpty()) {
        qDebug() << "No svg file set";
        return false;
    }

    QSvgGenerator svg;
    svg.setFileName(svgFile);
    QPainter p;
    p.begin(&svg);
    target->render(&p);
    p.end();
    
    return true;
}

void Renderer::start()
{
    if ( !svgFile.isEmpty() )
        renderToSvg();
}

