#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class HasherThread;

namespace Ui {
    class MainView;
}

class MainView : public QWidget {
    Q_OBJECT
public:
    MainView(QWidget *parent = 0);
    ~MainView();

public slots:
    void hashPressed();

private:
    Ui::MainView *ui;
    HasherThread *hasher;
};

#endif // MAINWINDOW_H
