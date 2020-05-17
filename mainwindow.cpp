#include "mainwindow.h"
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , cityView() {

    this->setGeometry(300, 200, 400, 400);
    this->view = new QGraphicsView();
    view->setScene(&this->cityView);

    this->setCentralWidget(view);
}

MainWindow::~MainWindow()
{
    delete view;
}

