#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTableWidget>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSqlDatabase db;

    QTableWidget *table;

    QLineEdit *nameEdit;
    QLineEdit *ageEdit;
    QLineEdit *salaryEdit;

    void connectDatabase();
    void retrieveData();

private slots:
    void insertData();
    void updateData();
    void deleteData();
};

#endif
