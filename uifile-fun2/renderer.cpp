#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QPrinter>
#include "renderer.h"

Renderer::Renderer(QObject *parent)
    : QObject(parent)
{
}

void Renderer::setTarget( QWidget *target )
{
    this->target = target;
}

void Renderer::setPdfFile( const QString &filename )
{
    pdfFile = filename;
}

bool Renderer::renderToPdf()
{
    if (pdfFile.isEmpty()) {
        qDebug() << "No PDF file set";
        return false;
    }

    QPrinter pdf;
    pdf.setOutputFormat( QPrinter::PdfFormat );
    pdf.setOutputFileName( pdfFile );
    QPainter p;
    p.begin(&pdf);
    target->render(&p);
    p.end();
    
    return true;
}

void Renderer::start()
{
    if ( !pdfFile.isEmpty() )
        renderToPdf();
}

