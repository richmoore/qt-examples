#ifndef VIEWER_H
#define VIEWER_H

#include "ui_viewer.h"

class Viewer : public QWidget
{
    Q_OBJECT

public:
    Viewer( QWidget *parent=0 );

public slots:
    void setImage( const QImage &image );

private:
    Ui::Viewer ui;
};

#endif // VIEWER_H

