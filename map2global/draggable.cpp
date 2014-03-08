#include <QMouseEvent>

#include "draggable.h"

Draggable::Draggable(QWidget *parent)
    : QLabel(parent)
{
    setText("Drag Me");
}

void Draggable::mousePressEvent(QMouseEvent *e)
{
    moving = true;
    last = e->globalPos();
}

void Draggable::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    last = e->globalPos();
    move(x()+dx, y()+dy);
}

void Draggable::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();

    move(x()+dx, y()+dy);
    moving = false;
}

void Draggable::moveEvent(QMoveEvent *e)
{
    Q_UNUSED(e);
    QPoint current(mapToGlobal(QPoint(0,0)));
    QString s("%1, %2");
    setText(s.arg(current.x()).arg(current.y()));
}

