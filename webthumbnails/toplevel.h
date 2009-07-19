#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <qwidget.h>

class QLabel;
class QLineEdit;
class KWebThumbnailer;

class TopLevel : public QWidget
{
    Q_OBJECT
public:
    TopLevel();

private slots:
    void reload();
    void completed(bool);

private:
    KWebThumbnailer *thumbnailer;
    QLineEdit *edit;
    QLabel *view;
};

#endif // TOPLEVEL_H

