#include "home.h"
#include "ui_home.h"

#include<qDebug>

const quint16 CRC16Table[256] = {    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
                                     0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
                                     0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
                                     0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
                                     0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
                                     0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
                                     0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
                                     0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
                                     0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
                                     0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
                                     0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
                                     0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
                                     0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
                                     0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
                                     0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
                                     0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
                                     0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
                                     0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
                                     0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
                                     0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
                                     0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
                                     0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
                                     0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
                                     0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
                                     0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
                                     0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
                                     0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
                                     0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
                                     0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
                                     0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
                                     0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
                                     0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0};
//本地crc16校验算法
quint16 dataVerificationCRC16(const quint8 *data, int len){
    int i;
    quint16 result =0;
    for (i = 0; i < len; i++)  {
       result = (result << 8) ^ CRC16Table[(result >> 8) ^ data[i]];
    }
    return result;
}


Home::Home(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Home),
    serialPort(new QSerialPort(this)),
    db(new Sqldatabase(this))
{
    ui->setupUi(this);

    pg = NULL;
    totalPackageLen = 0;
    currentDataLen = 0;
    flagAllEngineValueChange = 0;
    flagRecvPackage = 0;
    objectWidth = 0;

    this->setWindowTitle("智能分拣系统");
    this->setWindowIcon(QIcon(":/image/icon.ico"));

    getSerialPortInfo();
    initMovie();

    cameraInfoList = QCameraInfo::availableCameras();
    for(int i = 0; i < cameraInfoList.size(); i++){
        ui->comboBox->insertItem(i, cameraInfoList[i].description());

    }
    cameraViewFinder = new QCameraViewfinder(ui->widget_view);

    //信号和槽的手动连接
    connect(serialPort, &QSerialPort::readyRead, this, &Home::slotRecvSerialData);

}

Home::~Home()
{
    delete ui;
}

void Home::getSerialPortInfo()
{
    QList<QSerialPortInfo> seriaPortList = QSerialPortInfo::availablePorts();
    for(int i = 0; i < seriaPortList.size(); i++){
        QString portName = seriaPortList[i].portName();
        QString description = seriaPortList[i].description();
        QString manufacturer = seriaPortList[i].manufacturer();

        qDebug() << "portName" << portName << "description"
                 << description << "manufacturer" << manufacturer;

        //if(portName == "COM1")
        if(description == "USB-SERIAL CH340")
        {
            seriaPortInit(portName);
            ui->textBrowser->insertPlainText (portName);
            return ;
        }
    }

    //qDebug() << "初始化失败，没有符合条件的串口设备！";
    ui->textBrowser->insertPlainText("初始化失败，没有符合条件的串口设备！\n");
}

void Home::seriaPortInit(QString portName)
{
    serialPort->setPortName(portName);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    qDebug() << "串口初始化成功！";
    ui->textBrowser->insertPlainText("串口初始化成功！\n");
}

void Home::initMovie()
{
    label = new QLabel(this);
    movie = new QMovie(":/image/942.gif");
    label->setMovie(movie);
    movie->start();

    label->setScaledContents(true);
    label->resize(this->size());
    label->lower();
}

void Home::sendPackage(Package *pg)
{
    quint16 local_crc = dataVerificationCRC16((quint8 *)pg, PACKAGE_HEAD_LEN + pg->dataLength);
    pg->data[pg->dataLength] = local_crc;
    pg->data[pg->dataLength + 1] =local_crc >> 8;

    serialPort->write((char *)pg, PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN);

    if(pg != NULL){
        free(pg);
        pg = NULL;
    }
}

void Home::parsePackage(Package *pg)
{
    quint16 local_crc = dataVerificationCRC16((quint8 *)pg, PACKAGE_HEAD_LEN + pg->dataLength);
    quint16 *temp_crc = NULL;
    temp_crc = (quint16 *)&(pg->data[pg->dataLength]);

    /*
    quint16 recv_crc = 0;
    recv_crc |= (pg->data[pg->dataLength + 1] << 8);
    recv_crc |= pg->data[pg->dataLength];
    */
    if(local_crc == *(temp_crc)){
        ui->textBrowser->insertPlainText("CRC校验成功\n");
        ui->textBrowser->insertPlainText("function:");
        ui->textBrowser->insertPlainText(QString::number(pg->function, 16));
        ui->textBrowser->insertPlainText("\n");
        for(int i = 0; i < pg->dataLength; i++){
            ui->textBrowser->insertPlainText(QString::number(pg->data[i], 16));
            ui->textBrowser->insertPlainText(" ");
        }
        ui->textBrowser->insertPlainText("\n");

        if(pg->function == 0x07){
            if(ui->lineEdit->text() != "5"){
                ui->lineEdit->setText("5");
                ui->pushButton_Getdata->click();
            }
            else
                ui->pushButton_auto->click();
        }
        else if(pg->function == 0x08){
            ui->pushButton_auto->click();

        }

        //ui->pushButton_auto->click();
        ui->spinBox->setDisabled(false);
        ui->spinBox_2->setDisabled(false);
        ui->spinBox_3->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
        ui->spinBox_5->setDisabled(false);
        ui->spinBox_6->setDisabled(false);
    }
    else{
        ui->textBrowser->insertPlainText("CRC校验失败\n");
    }
}



/*
void Home::sendPackage(quint8 *data, quint16 dataLen)
{
    Package *pg;
    pg->deviceAddr = 0x00;
    pg->function = 0x00;
    pg->dataLength = dataLen;
    memcpy(pg->data, data, dataLen);
    quint16 local_crc = dataVerificationCRC16((quint8 *)pg, PACKAGE_HEAD_LENG + dataLen);
    pg->data[dataLen] = local_crc;
    pg->data[dataLen + 1] =local_crc >> 8;

    serialPort->write((char *)pg, PACKAGE_HEAD_LENG + dataLen + 2);
}
*/
void Home::paintEvent(QPaintEvent *pe)
{
    label->resize(this->size());
    if(ui->widget_view->width() > ui->widget_view->height() + 100){
        cameraViewFinder->resize(ui->widget_view->size());
    }

    QWidget::paintEvent(pe);
}

void Home::slotRecvSerialData()
{
    //保存本次串口中接收到的所有的数据
    QByteArray buf = serialPort->readAll();
    //保存上次接收到的数据
    Package *tmpPg = pg;
    //再开辟新的内存空间
    pg = (Package *)malloc(currentDataLen + buf.size());
    //复制上一次接受的数据
    memcpy(pg, (quint8 *)tmpPg, currentDataLen);
    if(tmpPg != NULL){
        free(tmpPg);
        tmpPg = NULL;
    }
    memcpy((quint8 *)pg + currentDataLen, buf.data(), buf.size());
    //计算本次接受到的数据长度
    currentDataLen += buf.size();
    if(currentDataLen >= PACKAGE_HEAD_LEN && totalPackageLen == 0){
        //计算数据包的长度
        totalPackageLen = PACKAGE_HEAD_LEN + pg->dataLength + PARITY_LEN;
    }
    //接受到的数据大于一个数据包了
    if(currentDataLen >= totalPackageLen && totalPackageLen != 0){
        Package *currentPg = (Package *)malloc(totalPackageLen);
        //currentPg就是一个完整数据包
        memcpy(currentPg, pg, totalPackageLen);
        //currentDataLen中保存下个数据包已经发送的数据长度
        currentDataLen -= totalPackageLen;
        Package *newTmpPg = pg;
        pg = (Package *)malloc(currentDataLen);
        //pg保存下一个数据包中部分数据
        memcpy(pg,(quint8 *)newTmpPg + totalPackageLen, currentDataLen);
        if(newTmpPg != NULL){
            free(newTmpPg);
            newTmpPg =NULL;
        }

        //解析数据包
        //serialPort->write((char *)currentPg, totalPackageLen);
        parsePackage(currentPg);
        totalPackageLen = 0;
    }


/*
    QByteArray buf = serialPort->readAll();
    QString str = QString("recvSerialData:") + buf + "\n";

    ui->textBrowser->insertPlainText(str);
*/
}

void Home::slotSaveImage(int id, const QImage preview)
{
    Q_UNUSED(id);
    QPixmap pix;
    pix = pix.fromImage(preview);

    pix = pix.scaled(ui->label_image->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //ui->label_image->setPixmap(pix);
    QImage img = pix.toImage();
    img.save("armtest.png");

    QString path = "armtest.png";
    Mat imgcv3 = imread(path.toStdString());
    if(imgcv3.empty()){
        qDebug() << "图片数据错误";
        return;
    }
    Mat imgcv;
    cv::resize(imgcv3, imgcv, Size(318, 266));

    Mat imgdil = imageInit(imgcv);
    getContours(imgdil, imgcv);

    Mat imgcv2;
    cvtColor(imgcv, imgcv2, COLOR_BGR2RGB);

    QImage imgDisplay = QImage(imgcv2.data, imgcv2.cols, imgcv2.rows, imgcv2.cols * imgcv2.channels(), QImage::Format_RGB888);
    imgDisplay = imgDisplay.scaled(ui->label_image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //显示图片
    ui->label_image->setPixmap(QPixmap::fromImage(imgDisplay));
//    imshow("armtest", imgcv);
//    imshow("armtest2",imgdil);
}

Mat Home::imageInit(Mat imgcv)
{
    Mat imgGray, imgBlur, imgCanny, imgDil;
    cvtColor(imgcv, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);
    Canny(imgBlur, imgCanny, 50, 150);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(imgCanny, imgDil, kernel);
    return imgDil;
}

void Home::getContours(Mat imgDil, Mat imgcv)
{
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    line(imgcv, Point(0, 133), Point(317, 133), Scalar(0, 255, 255), 3);
    line(imgcv, Point(159, 0), Point(159, 265), Scalar(0, 255, 255), 3);
/*
    if(contours.size() == 0){
        Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + 0 + PARITY_LEN);
        pg->deviceAddr = 0x00;
        pg->function = 0x07;// 移动传送带指令
        pg->dataLength = 0;
        sendPackage(pg);
    }
*/

    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);
        //ui->textBrowser->insertPlainText(QString("面积：") + QString::number(area) + "\n");

        //排除干扰
//        if(area < 1000){
//            continue;
//        }

        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boudRect(contours.size());
        float peri = arcLength(contours[i], true);
        approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
        boudRect[i] = boundingRect(conPoly[i]);
        drawContours(imgcv, conPoly, i, Scalar(255, 0, 255), 2);
        rectangle(imgcv, boudRect[i].tl(), boudRect[i].br(), Scalar(0, 255, 0), 5);
        ui->textBrowser->insertPlainText(QString("坐标：") + "(" + QString::number(boudRect[i].tl().x) + ","
                                         + QString::number(boudRect[i].tl().y) + ")\n");
        int objx = (boudRect[i].tl().x + boudRect[i].br().x) / 2;
        int objy = (boudRect[i].tl().y + boudRect[i].br().y) / 2;
        circle(imgcv, Point(objx, objy), 5, Scalar(0, 255, 0), 3, FILLED);

//        if(0 < objx && objx <=159 && 0 < objy && objy <= 133){
//            ui->lineEdit->setText("1");
//        }
//        else if(159 < objx && objx <=317 && 0 < objy && objy <= 133){
//            ui->lineEdit->setText("2");
//        }
//        else if(0 < objx && objx <=159 && 133 < objy && objy <= 256){
//            ui->lineEdit->setText("3");
//        }
//        else if(159 < objx && objx <=317 && 133 < objy && objy <= 256){
//            ui->lineEdit->setText("4");
//        }
//        else{
//            qDebug() << "坐标无效";
//        }

//        ui->pushButton_Getdata->click();
//        //抓一个物体就退出
//        break;
    }

    putText(imgcv, "1", Point(6, 6 + 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255), 1);
    putText(imgcv, "2", Point(166, 6 + 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255), 1);
    putText(imgcv, "3", Point(6, 139 + 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255), 1);
    putText(imgcv, "4", Point(166, 139 + 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 0, 255), 1);

    int flagThing = 0;
    for(int i = 0; i < contours.size(); i++){
        int area = contourArea(contours[i]);
        //ui->textBrowser->insertPlainText(QString("面积：") + QString::number(area) + "\n");

        //排除干扰
        if(area < 1000){
            continue;
        }

        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boudRect(contours.size());
        float peri = arcLength(contours[i], true);
        approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
        boudRect[i] = boundingRect(conPoly[i]);
//        drawContours(imgcv, conPoly, i, Scalar(255, 0, 255), 2);
//        rectangle(imgcv, boudRect[i].tl(), boudRect[i].br(), Scalar(0, 255, 0), 5);
//        ui->textBrowser->insertPlainText(QString("坐标：") + "(" + QString::number(boudRect[i].tl().x) + ","
//                                         + QString::number(boudRect[i].tl().y) + ")\n");
        int objx = (boudRect[i].tl().x + boudRect[i].br().x) / 2;
        int objy = (boudRect[i].tl().y + boudRect[i].br().y) / 2;
//        circle(imgcv, Point(objx, objy), 5, Scalar(0, 255, 0), 3, FILLED);

        if(0 < objx && objx <=159 && 0 < objy && objy <= 133){
            ui->lineEdit->setText("1");
        }
        else if(159 < objx && objx <=317 && 0 < objy && objy <= 133){
            ui->lineEdit->setText("2");
        }
        else if(0 < objx && objx <=159 && 133 < objy && objy <= 256){
            ui->lineEdit->setText("3");
        }
        else if(159 < objx && objx <=317 && 133 < objy && objy <= 256){
            ui->lineEdit->setText("4");
        }
        else{
            qDebug() << "坐标无效";
        }

        objectWidth = boudRect[i].width;
        ui->textBrowser->insertPlainText(QString("物体宽度：") + QString::number(objectWidth) + "\n");
        ui->pushButton_Getdata->click();
        flagThing = 1;
        //抓一个物体就退出
        break;
    }

    if(flagThing == 0){
        objectWidth = 0;
        Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + 0 + PARITY_LEN);
        pg->deviceAddr = 0x00;
        pg->function = 0x08;// 移动传送带指令
        pg->dataLength = 0;
        sendPackage(pg);
    }
}


void Home::on_pushButton_SerialConnnect_clicked()
{
    if(serialPort->isOpen()){
        //qDebug() << "串口已经打开！";
        ui->textBrowser->insertPlainText("串口已经打开！\n");
    }
    else{
        if(serialPort->open(QIODevice::ReadWrite)){
            QString str = "hello COM2";
            //qDebug() << "串口打开成功";
            ui->textBrowser->insertPlainText("串口打开成功\n");
           // serialPort->write(str.toStdString().c_str(), strlen(str.toStdString().c_str()));
        }
        else{
            //qDebug() << "串口打开失败";
            ui->textBrowser->insertPlainText("串口打开失败\n");
        }
    }
}

void Home::on_pushButton_Insert_clicked()
{
    if(ui->lineEdit->text() == ""){
        ui->textBrowser->insertPlainText("请输入区域编号\n");
        return;
    }
    Data data;
    data.areaNum = ui->lineEdit->text().toInt();
    data.engine[0] = ui->spinBox->value();
    data.engine[1] = ui->spinBox_2->value();
    data.engine[2] = ui->spinBox_3->value();
    data.engine[3] = ui->spinBox_4->value();
    data.engine[4] = ui->spinBox_5->value();
    data.engine[5] = ui->spinBox_6->value();

    if(db->databaseInsert(data)){
        ui->textBrowser->insertPlainText("记录操作成功\n");
    }
}

void Home::on_pushButton_Getdata_clicked()
{
    if(ui->lineEdit->text() == ""){
        ui->textBrowser->insertPlainText("请输入区域编号\n");
        return;
    }
    Data data;

    data = db->databaseGetdata(ui->lineEdit->text().toInt());
    if(data.areaNum == -1){
        qDebug() << "获取数据失败";
        return;
    }
    flagAllEngineValueChange = 1;
/*
    for(int i=0; i < 6; i++){
        qDebug() << data.engine[i] <<" ";
    }

    quint32 testData = 0x123456;
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + 3 + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x00;
    pg->dataLength = 3;
    memcpy(pg->data, (quint8 *)&testData, pg->dataLength);
    sendPackage(pg);
*/

    if(ui->lineEdit->text() == "5")
    {
        data.engine[5] = 1000;
    }
    else{
        data.engine[5] = 2000 - (objectWidth / 300.0) * 1000;
    }
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(data.engine) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x07;// 所有舵机参数
    pg->dataLength = sizeof(data.engine);
    memcpy(pg->data, (quint8 *)data.engine, sizeof(data.engine));

    ui->spinBox->setValue(data.engine[0]);
    ui->spinBox_2->setValue(data.engine[1]);
    ui->spinBox_6->setValue(data.engine[2]);
    ui->spinBox_4->setValue(data.engine[3]);
    ui->spinBox_3->setValue(data.engine[4]);
    ui->spinBox_5->setValue(data.engine[5]);
    sendPackage(pg);
    flagAllEngineValueChange = 0;
}



void Home::on_spinBox_valueChanged(int arg1)
{
    if(flagAllEngineValueChange == 1){
        return;
    }

    ui->spinBox->setDisabled(true);
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(arg1) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x01;//舵机编号
    pg->dataLength = sizeof(arg1);
    memcpy(pg->data, (quint8 *)&arg1, sizeof(arg1));
    sendPackage(pg);

}



void Home::on_spinBox_2_valueChanged(int arg1)
{
    if(flagAllEngineValueChange == 1){
        return;
    }
    ui->spinBox_2->setDisabled(true);
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(arg1) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x02;//舵机编号
    pg->dataLength = sizeof(arg1);
    memcpy(pg->data, (quint8 *)&arg1, sizeof(arg1));
    sendPackage(pg);
}

void Home::on_spinBox_6_valueChanged(int arg1)
{
    if(flagAllEngineValueChange == 1){
        return;
    }
    ui->spinBox_6->setDisabled(true);
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(arg1) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x03;//舵机编号
    pg->dataLength = sizeof(arg1);
    memcpy(pg->data, (quint8 *)&arg1, sizeof(arg1));
    sendPackage(pg);
}

void Home::on_spinBox_4_valueChanged(int arg1)
{
    if(flagAllEngineValueChange == 1){
        return;
    }
    ui->spinBox_4->setDisabled(true);
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(arg1) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x04;//舵机编号
    pg->dataLength = sizeof(arg1);
    memcpy(pg->data, (quint8 *)&arg1, sizeof(arg1));
    sendPackage(pg);
}

void Home::on_spinBox_3_valueChanged(int arg1)
{
    if(flagAllEngineValueChange == 1){
        return;
    }
    ui->spinBox_3->setDisabled(true);
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(arg1) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x05;//舵机编号
    pg->dataLength = sizeof(arg1);
    memcpy(pg->data, (quint8 *)&arg1, sizeof(arg1));
    sendPackage(pg);
}

void Home::on_spinBox_5_valueChanged(int arg1)
{
    if(flagAllEngineValueChange == 1){
        return;
    }
    ui->spinBox_5->setDisabled(true);
    Package *pg = (Package *)malloc(PACKAGE_HEAD_LEN + sizeof(arg1) + PARITY_LEN);
    pg->deviceAddr = 0x00;
    pg->function = 0x06;//舵机编号
    pg->dataLength = sizeof(arg1);
    memcpy(pg->data, (quint8 *)&arg1, sizeof(arg1));
    sendPackage(pg);
}

void Home::on_pushButton_opencamera_clicked()
{
    camera = new QCamera(cameraInfoList[ui->comboBox->currentIndex()]);
    camera->setCaptureMode(QCamera::CaptureVideo);
    camera->setViewfinder(cameraViewFinder);
    cameraViewFinder->resize(ui->widget_view->size());
    cameraImageCapture = new QCameraImageCapture(camera);
    camera->start();

    connect(cameraImageCapture, &QCameraImageCapture::imageCaptured, this, &Home::slotSaveImage);

}

void Home::on_pushButton_auto_clicked()
{
    cameraImageCapture->capture();
}

