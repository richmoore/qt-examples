#include "viewer.h"

Viewer::Viewer( QWidget *parent )
    : QWidget(parent)
{
    ui.setupUi(this);
}

void Viewer::setImage( const QImage &image )
{
    ui.label->setPixmap( QPixmap::fromImage(image) );
}
