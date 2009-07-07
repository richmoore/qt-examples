#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class MediaWiki;
class QLineEdit;
class QListWidget;
class QVBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void go();

private slots:
    void finished(bool);

private:
    MediaWiki *wiki;

    QLineEdit *edit;
    QListWidget *list;
    QVBoxLayout *layout;
};

#endif // MAINWINDOW_H
