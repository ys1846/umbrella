#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "home.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonLogin_clicked();

    void on_lineEditKey_returnPressed();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
