#ifndef HOME_H
#define HOME_H

#include <QMainWindow>

#include<QSerialPortInfo>
#include<QSerialPort>

#include<QLabel>
#include<QMovie>
#include<QPaintEvent>

#include"sqldatabase.h"

#define PACKAGE_HEAD_LEN 4
#define PARITY_LEN 2

#include<QCamera>
#include<QCameraInfo>
#include<QCameraImageCapture>
#include<QCameraViewfinder>

#include"opencv.hpp"

using namespace std;
using namespace cv;

// 地址码1/功能码1/数据长度2/数据(不定)/效验码2

//数据包类型
struct Package{
    quint8 deviceAddr;
    quint8 function;
    quint16 dataLength;
    quint8 data[];

};

namespace Ui {
class Home;
}

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = 0);
    ~Home();

    void getSerialPortInfo();
    void seriaPortInit(QString portName);
    void initMovie();
    //void sendPackage(quint8 * data, quint16 dataLen);
    void sendPackage(Package *pg);
    void parsePackage(Package *pg);

    Mat imageInit(Mat imgcv);
    void getContours(Mat imgDil, Mat imgcv);

public slots:
    void paintEvent(QPaintEvent *pe);
    void slotRecvSerialData();
    void slotSaveImage(int id, const QImage preview);

private slots:


    void on_pushButton_SerialConnnect_clicked();

    void on_pushButton_Insert_clicked();

    void on_pushButton_Getdata_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_spinBox_6_valueChanged(int arg1);

    void on_spinBox_4_valueChanged(int arg1);

    void on_spinBox_3_valueChanged(int arg1);

    void on_spinBox_5_valueChanged(int arg1);

    void on_pushButton_opencamera_clicked();

    void on_pushButton_auto_clicked();


private:
    Ui::Home *ui;

    QSerialPort *serialPort;
    QLabel *label;
    QMovie *movie;

    Sqldatabase *db;

    //保存数据
    Package *pg;
    //保存从串口中接收到的数据的字节数
    int currentDataLen;
    //当前数据包总长度
    int totalPackageLen;

    int flagAllEngineValueChange;
    int flagRecvPackage;

    QList<QCameraInfo> cameraInfoList;
    QCamera *camera;
    QCameraViewfinder *cameraViewFinder;
    QCameraImageCapture *cameraImageCapture;

    int objectWidth;

};

#endif // HOME_H
