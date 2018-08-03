#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QStringList>
#include <QMessageBox>

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

    ui->btnOpen->setText("打开串口");


}

void MainWindow::on_btnOpen_clicked()
{
    if( ui->btnOpen->text() == "打开串口")
    {
       my_serialport = new QSerialPort(this);
       my_serialport->setPortName( ui->cmbPortName->currentText() );
       if(my_serialport->open(QIODevice::ReadWrite))
       {
           //设置波特率
           my_serialport->setBaudRate(ui->cmbBuadRate->currentText().toInt());
           //设置数据位
           my_serialport->setDataBits(QSerialPort::Data8);
           //设置校验位
           my_serialport->setParity(QSerialPort::NoParity);
           //设置流控制
           my_serialport->setFlowControl(QSerialPort::NoFlowControl);
           //设置停止位
           my_serialport->setStopBits(QSerialPort::OneStop);
           ui->btnOpen->setText(tr("关闭串口"));
           //关闭设置菜单使能
          ui->cmbPortName->setEnabled(false);
          ui->cmbBuadRate->setEnabled(false);
          ui->cmbDataBits->setEnabled(false);
          ui->cmbParity->setEnabled(false);
          ui->cmbStopBits->setEnabled(false);
          ui->cmbFlowBits->setEnabled( false );

          ui->btnOpen->setText(tr("关闭串口"));
          ui->btnSend->setEnabled(true);
          QObject::connect(my_serialport, &QSerialPort::readyRead, this, &MainWindow::Read_Data);
       }
       else
       {
           QMessageBox::about(NULL, "提示", "串口没有打开！");
           return;
       }
    }
    else
    {
        my_serialport->close();
        ui->cmbPortName->setEnabled(true);
        ui->cmbBuadRate->setEnabled(true);
        ui->cmbDataBits->setEnabled(true);
        ui->cmbParity->setEnabled(true);
        ui->cmbStopBits->setEnabled(true);
        ui->cmbFlowBits->setEnabled( true );
        ui->btnOpen->setText(tr("打开串口"));
        ui->btnSend->setEnabled(false);
    }

}

void MainWindow::Read_Data()
{
    QByteArray buf;
    buf = my_serialport->readAll();
    if(!buf.isEmpty())
    {
        QString str = ui->te_receive->toPlainText();
        str += buf;
        ui->te_receive->clear();
        ui->te_receive->append(str);
    }
    buf.clear();
}

void MainWindow::on_btnSend_clicked()
{
  qDebug()<< my_serialport->write(ui->teSend->toPlainText().toLatin1());
}
