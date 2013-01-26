#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <QLabel>

class Draggable : public QLabel
{
    Q_OBJECT

public:
    Draggable(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool moving;
    QPoint last;
};

#endif // DRAGGABLE_H

