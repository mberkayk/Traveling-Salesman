#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "cityview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QGraphicsView *view;
    CityView cityView;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
