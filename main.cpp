#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.setWindowTitle("SQLite CRUD Application");
    w.resize(700, 400);
    w.show();

    return a.exec();
}
