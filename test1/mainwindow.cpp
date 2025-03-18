#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");
    this->setWindowIcon(QIcon(":/image/icon.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonLogin_clicked()
{
//    QString id = ui->lineEditID->text();
//    qDebug() << "id:"<< id;

    //获取账号密码，验证，正确打印成功，错误打印错误
    QString id = ui->lineEditID->text();
    QString key = ui->lineEditKey->text();

    //if(id == "qq" && key == "123"){
    if(1){
        Home *h = new Home;
        h->show();
        this->close();
        qDebug() << "登录成功";
    }
    else qDebug() <<"登录失败";

}

void MainWindow::on_lineEditKey_returnPressed()
{
    on_pushButtonLogin_clicked();
}
