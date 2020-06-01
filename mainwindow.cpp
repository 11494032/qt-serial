#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QStringList>
#include <QMessageBox>
#include <QLabel>
#include "settingsdialog.h"
#include "console.h"
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_settings(new SettingsDialog),
    m_serial(new QSerialPort(this)),
    m_console(new Console)
{
    ui->setupUi(this);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    ui->statusBar->addWidget(m_status);
    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this,&MainWindow::readData);
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
  //  m_serial_thread = new SerialThread();

   // connect(m_serial_thread, &SerialThread::TestSignal, this,  &MainWindow::DisplayMsg);

    //执行子线程
  //  m_serial_thread->start();
   // connect(m_serial_thread, SIGNAL(&SerialThread::TestSignal(int)), this, SLOT(&MainWindow::DisplayMsg(int)));

  //  connect(m_serial_thread,&SerialThread::TestSignal,this,&MainWindow::DisplayMsg,Qt::AutoConnection);

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DisplayMsg(int a)
{
    ui->te_receive->append(QString::number(a));
}


//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
  //  m_console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About uart Terminal"),
                       tr("author:yxl"));
}

void MainWindow::clear()
{
    ui->te_receive->clear();
}

void MainWindow::writeData(const QByteArray &data)
{
    if( !m_serial->isOpen()){
        QMessageBox::warning(this, tr("串口开启提示"),
                           tr("请选择串口"));
    }


    m_serial->write(data);
}

int asciitonum( char data ){
    int ret = 0;
//    qDebug()<<""<<data;
    if( data >='0' && data <='9' )
        ret = data-'0';
    else if (data >='a' && data <='f') {
        ret = data-'a'+10;
    }
    else if (data >='A' && data <='F') {
        ret = data-'A'+10;
    }
  //   qDebug()<<"----"<<ret;
    return ret;
}

void MainWindow:: handle_data( const QByteArray data){
    static int rev[8] = {0,};
    static bool is_start = false;
    int res = 0;
    static int index = 0;
    for(  int i = 0; i<data.length();i += 2){
        res = (asciitonum(data.at(i))<<4 | asciitonum(data.at(i+1)))& 0x00FF;
        qDebug()<<"receive info:"<<data.length()<<data.at(i)<<data.at(i+1)<<res;
        if(res == 0x5A){
            is_start = true;
            index = 0;
        }
        if(is_start){
            rev[index]= res;
            index++;
        }
        if(index >= 7 ){
            is_start = false;
        }
    }
    if(rev[5] == 0x57 )
    {
        if( rev[2] == 0x10 && rev[3] == 0x01){ //接收到上升指令，开始上升
            ui->status_lineEdit->setText("关锁中");
        }
        else if( rev[2] == 0x10 && rev[3] == 0x02){//接收到下降指令，开始下降
            ui->status_lineEdit->setText("开锁中");
        }
        else if( rev[2] == 0x10 && rev[3] == 0x11){//车位锁上升完成
            ui->status_lineEdit->setText("锁已关");
        }
        else if( rev[2] == 0x10 && rev[3] == 0x22){//车位锁下降完成
            ui->status_lineEdit->setText("锁已开");
        }
        else if( rev[2] == 0x20 && rev[3] == 0x00){//车位锁处于下降状态
            ui->status_lineEdit->setText("锁处于开锁状态");
        }
        else if( rev[2] == 0x20 && rev[3] == 0x01){//车位锁处于上升状态
            ui->status_lineEdit->setText("锁处于关锁状态");
        }
        memset(rev,0,sizeof (rev));
        qDebug()<<"receive info success:"<<rev[2] << rev[3];
    }
}
void MainWindow::readData()
{
    QDateTime curDateTime=QDateTime::currentDateTime();
    const QByteArray data = m_serial->readAll();
    if( !ui->hexRecvcheckBox->isChecked() ){
        ui->te_receive->insertPlainText(data);
    }
    else if( ui->hexRecvcheckBox->isChecked() ){
        QByteArray hexData = data.toHex();
        QString string;
        string.prepend(hexData);// QByteArray转QString方法2
        //qDebug()<<"receive info:"<<hexData.length();
       // handle_data(hexData);
        m_console->putData(hexData);
        ui->te_receive->insertPlainText(string);

    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clear);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->sendpushButton, &QPushButton::pressed,this,&MainWindow::buttonSendData);

}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}




void MainWindow::buttonSendData()
{

    QString sendStr = ui->teSend->toPlainText();

    if( ui->hexsendcheckBox->isChecked() == false){
        for(int i = 0;i < sendStr.length();++i){
            QChar t = sendStr.at(i);
             qDebug()<<"hello "<<t<<"!"<<endl;
        }
         m_serial->write(sendStr.toLocal8Bit());
    }
   else if( ui->hexsendcheckBox->isChecked()){

            if (sendStr.contains(" "))

            {

                sendStr.replace(QString(" "),QString(""));//把空格去掉

            }

            qDebug()<<"Write to serial: "<<sendStr;
            //转换成16进制发送

            QByteArray sendBuf = QByteArray::fromHex(sendStr.toLatin1());
             m_serial->write(sendBuf);
    }
}


void MainWindow::on_CloselockpushButton_clicked()
{
    QByteArray array;
    array[0] = 0x5A;
    array[1] = 0x00;
    array[2] = 0x01;
    array[3] = 0x01;
    array[4] = 0x00;
    array[5] = 0x57;
  this->writeData(array);
}

void MainWindow::on_openlockpushButton_clicked()
{
    QByteArray array;
    array[0] = 0x5A;
    array[1] = 0x00;
    array[2] = 0x01;
    array[3] = 0x02;
    array[4] = 0x00;
    array[5] = 0x57;
   this->writeData(array);
}

void MainWindow::on_QuerypushButton_clicked()
{
    QByteArray array;
    array[0] = 0x5A;
    array[1] = 0x00;
    array[2] = 0x02;
    array[3] = 0x01;
    array[4] = 0x00;
    array[5] = 0x57;
   this->writeData(array);
}

void MainWindow::on_alarmpushButton_clicked()
{
    QByteArray array;
    array[0] = 0x5A;
    array[1] = 0x00;
    array[2] = 0x01;
    array[3] = 0x03;
    array[4] = 0x00;
    array[5] = 0x57;
   this->writeData(array);
}

void MainWindow::on_CloseloclearalarmckpushButton_clicked()
{
    QByteArray array;
    array[0] = 0x5A;
    array[1] = 0x00;
    array[2] = 0x01;
    array[3] = 0x04;
    array[4] = 0x00;
    array[5] = 0x57;
   this->writeData(array);
}
