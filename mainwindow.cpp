#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug()<<"Name:"<<info.portName();

        //这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            //将串口号添加到cmb
            ui->cmbPortName->addItem(info.portName());
            //关闭串口等待人为(打开串口按钮)打开
            serial.close();
        }

    }

    QStringList baudList;//波特率
    QStringList parityList;//校验位
    QStringList dataBitsList;//数据位
    QStringList stopBitsList;//停止位

    baudList<<"50"<<"75"<<"100"<<"134"<<"150"<<"200"<<"300"
              <<"600"<<"1200"<<"1800"<<"2400"<<"4800"<<"9600"
             <<"14400"<<"19200"<<"38400"<<"56000"<<"57600"
            <<"76800"<<"115200"<<"128000"<<"256000";

    ui->cmbBuadRate->addItems(baudList);
    ui->cmbBuadRate->setCurrentIndex(12);

    parityList<<"无"<<"奇"<<"偶";
    parityList<<"标志";
    parityList<<"空格";

    ui->cmbParity->addItems(parityList);
    ui->cmbParity->setCurrentIndex(0);

    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->cmbDataBits->addItems(dataBitsList);
    ui->cmbDataBits->setCurrentIndex(3);

    stopBitsList<<"1";
    stopBitsList<<"1.5";
    stopBitsList<<"2";

    ui->cmbStopBits->addItems(stopBitsList);
    ui->cmbStopBits->setCurrentIndex(0);

    //设置按钮可以被按下
    ui->btnOpen->setCheckable(true);


}

void MainWindow::on_btnOpen_clicked()
{

}
