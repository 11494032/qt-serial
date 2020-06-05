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
#include <QHBoxLayout>
#include <QLineEdit>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_settings(new SettingsDialog),
    m_serial(new QSerialPort(this)),
    m_console(new Console)
{
    ui->setupUi(this);

    QFile file("blue.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    ui->statusBar->addWidget(m_status);
    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this,&MainWindow::readData);
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
    connect(ui->wdtPushButton, &QPushButton::clicked,this,&MainWindow::wdtPushbutton);
    connect(ui->spiPushButton, &QPushButton::clicked,this,&MainWindow::spiPushbutton);
    connect(ui->radarPushButton, &QPushButton::clicked,this,&MainWindow::radarPushbutton);
    connect(ui->beerPushButton, &QPushButton::clicked,this,&MainWindow::beerPushbutton);
    connect(ui->iccidPushButton, &QPushButton::clicked,this,&MainWindow::iccidPushbutton);
    connect(ui->macPushButton, &QPushButton::clicked,this,&MainWindow::macPushbutton);
    connect(m_console, &Console::TestSignal, this, &MainWindow::interfaceShow);
    connect(m_console, &Console::stringSignal, this, &MainWindow::interfaceStringShow);

    init();
}

void MainWindow::interfaceStringShow(int i,QString data){
   //qDebug("signeal:%x %s\n",i,(char)data);
  if( i == 5 ){ //iccid

         addParamItem(5,2,goodsModel,data);
   }
  if( i == 7 ){ //
         addParamItem(7,2,goodsModel,data);
   }
  else if( i == 41 ){
          ui->readSNlineEdit->setText(data);
  }
  else if( i == 44 ){
            ui->readVersionlineEdit->setText(data);
  }

}
void MainWindow::interfaceShow(int i,long data){
  // qDebug("signeal:%x %lx\n",i,data);
   if( i == 99 ){ //看门狗
    ui->startlabel->setText("开始测试");
    addParamItem(0,2,goodsModel,"成功");
   }
   else if( i == 1 && data == 1){ //看门狗
    addParamItem(1,2,goodsModel,"成功");
   // goodsModel->item(0, 2)->setForeground(QBrush(QColor(255, 0, 0)));
   }
   else if( i == 2 ){ //spi
       if( data == 1){
            addParamItem(2,2,goodsModel,"成功");
       }
       else{
        addParamItem(2,2,goodsModel,"失败");
        goodsModel->item(0, 2)->setForeground(QBrush(QColor(255, 0, 0)));
       }
   }
   else if( i == 4 && data > 0){
            addParamItem(4,2,goodsModel,QString::number(data));

   }
   else if( i == 6 && data > 0){
            if(data == 1)
            addParamItem(6,2,goodsModel,"联网中");
   }
   else if( i == 61 ){
            addParamItem(6,2,goodsModel,"联网成功，信号值："+QString::number(data));
   }
   else if( i == 62 ){
            addParamItem(6,2,goodsModel,"联网失败，失败原因："+QString::number(data));
   }
   else if( i == 40 ){
           if(data ==1 ){
             ui->writeSnlineEdit->setText("SN写成功");
           }
           if(data ==2 ){
             ui->writeSnlineEdit->setText("SN写失败");
           //  ui->writeSnlineEdit->setStyleSheet("color:red");
           }
   }
   else if( i == 42 ){
           if(data ==1 ){
             ui->writeImeilineEdit->setText("imei写成功");
           }
           if(data ==2 ){
             ui->writeImeilineEdit->setText("imei写失败");
           //  ui->writeSnlineEdit->setStyleSheet("color:red");
           }
   }
   else if( i == 43 ){
           if(data ==1 ){
             ui->worklineEdit->setText("模式写成功");
           }
           if(data ==2 ){
             ui->worklineEdit->setText("模式写失败");
           //  ui->writeSnlineEdit->setStyleSheet("color:red");
           }
   }


}

void MainWindow::wdtPushbutton(  ){
    QByteArray data;
    data[0] = 0xAC;
    data[1] = 0x02;
    data[2] = 0x01;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x03;
    data[6] = 0xCC;
    this->writeData(data);
}

void MainWindow::spiPushbutton(){
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x02;
    data[2] = 0x02;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x00;
    data[6] = 0xCC;
    this->writeData(data);
}

void MainWindow::radarPushbutton(){
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x02;
    data[2] = 0x04;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x06;
    data[6] = 0xCC;
    this->writeData(data);
}

void MainWindow::beerPushbutton(){
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x02;
    data[2] = 0x03;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x01;
    data[6] = 0xCC;
    this->writeData(data);
}

void MainWindow::iccidPushbutton(){
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x02;
    data[2] = 0x06;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x04;
    data[6] = 0xCC;
    this->writeData(data);
}
void MainWindow::macPushbutton(){
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x03;
    data[2] = 0x03;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x00;
    data[6] = 0xCC;
    this->writeData(data);
}

void MainWindow::init()
{
    ui->treeView->setEditTriggers(0);
    ui->treeView->setRootIsDecorated(false);
    ui->treeView->setAlternatingRowColors(true);
    goodsModel = new QStandardItemModel(0, 4,this);
    ui->treeView->setColumnWidth(0,100);
    ui->treeView->setColumnWidth(1,800);
    ui->treeView->setColumnWidth(2,1300);
    ui->treeView->setColumnWidth(3,800);

    ui->treeView->setModel(goodsModel);
    createSubjectModel(goodsModel);
}
void MainWindow::addSubject(int row,QStandardItemModel *model, const QString &subject)
{
    model->insertRow(row);
    model->setData(model->index(row, 0), row);
    model->setData(model->index(row, 1), subject);
}

void MainWindow::addParamItem(int row, int col,QStandardItemModel *model, const QString &desc)
{
    model->setData(model->index(row, col), desc);
}

void MainWindow:: createSubjectModel( QStandardItemModel *model )
{
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("序号"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("测试项"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("测试结果"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("参考"));

    addSubject(0,model, "RS485测试" );
    addSubject(1,model, "看门狗" );
    addSubject(2,model, "spiflash" );
    addSubject(3,model, "蜂鸣器" );
    addSubject(4,model, "雷达测试" );
    addSubject(5,model, "ICCID" );
    addSubject(6,model, "网络质量" );
    addSubject(7,model, "蓝牙MAC" );
    addParamItem(4,3,goodsModel,"1.无车站位: <1000000 正常\n2.有车站位:>=2000000为正常\n3.仅当1,2都正常时，雷达模块工作正常");
    addParamItem(6,3,goodsModel,"联网失败：\n03H :读卡失败; \n04H :SOCKET失败;\n05H :连接失败");

}
void MainWindow::showClick(QModelIndex index)
{
    QString strTemp;
    strTemp = index.data().toString();

    QMessageBox msg;
    msg.setText(strTemp);
    msg.exec();


}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->startlabel->clear();
    for( int i = 0; i<8; i++){
        addParamItem(i,2,goodsModel,"");
    }

}

void MainWindow::writeData(const QByteArray &data)
{
    if( !m_serial->isOpen()){
        QMessageBox::warning(this, tr("串口开启提示"),
                           tr("请选择串口"));
    }


    m_serial->write(data);
}


void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();
    QByteArray hexData = data.toHex();
    QString string;
    string.prepend(hexData);// QByteArray转QString方法2
    m_console->putData(hexData);


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

}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}




void MainWindow::on_writeSNPushButton_clicked()
{
   QByteArray writedata;

   QString data = ui->writeSnlineEdit->text();

   QByteArray bytes = data.toLatin1();
   //qDebug("signeal:%x %x\n",bytes.at(0),bytes.at(1));
 //  writeData()
   if( bytes.size() != 16){
        ui->writeSnlineEdit->setText("数据长度错误");
       return;
   }
   int length = bytes.size();
   writedata.resize(6+length);
   writedata[5+bytes.size()] = 0;
   writedata[0] = 0xAC;
   writedata[1] = 0x02;
   writedata[2] = 0x09;
   writedata[3] = bytes.size();

   for( int i = 0; i < bytes.size(); i++ ){
        writedata[4+i] = bytes.at(i);
        writedata[5+length] = ( writedata[5+length]^writedata[4+i]);
   }
   writedata[4+length] = (writedata[5+length]^writedata[1]^writedata[2]^writedata[3]);
   writedata[5+length] = 0xCC;
   this->writeData(writedata);
}

void MainWindow::on_readSnpushButton_clicked()
{
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x03;
    data[2] = 0x01;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x02;
    data[6] = 0xCC;
    this->writeData(data);
}

void MainWindow::on_writeImeipushButton_clicked()
{
    QByteArray writedata;

    QString data = ui->writeImeilineEdit->text();
    if(data.isEmpty() ){
        ui->writeImeilineEdit->setText("数据长度错误");
        return;
    }
    QByteArray bytes = data.toLatin1();
    //qDebug("signeal:%x %x\n",bytes.at(0),bytes.at(1));
  //  writeData()
    if( bytes.size() != 15){
         ui->writeImeilineEdit->setText("数据长度错误");
        return;
    }
    int length = bytes.size();
    writedata.resize(6+length);
    writedata[5+bytes.size()] = 0;
    writedata[0] = 0xAC;
    writedata[1] = 0x02;
    writedata[2] = 0x07;
    writedata[3] = bytes.size();

    for( int i = 0; i < bytes.size(); i++ ){
         writedata[4+i] = bytes.at(i);
         writedata[5+length] = ( writedata[5+length]^writedata[4+i]);
    }
    writedata[4+length] = (writedata[5+length]^writedata[1]^writedata[2]^writedata[3]);
    writedata[5+length] = 0xCC;
    this->writeData(writedata);
}

void MainWindow::on_workpushButton_clicked()
{
    QByteArray writedata;

    QString data = ui->worklineEdit->text();
    if(data.isEmpty() ){
        ui->worklineEdit->setText("数据长度错误");
        return;
    }
    QByteArray bytes = data.toLatin1();
    //qDebug("signeal:%x %x\n",bytes.at(0),bytes.at(1));
  //  writeData()
    if( ( bytes.size() != 1 || ( bytes.at(0) != 0x32 && bytes.at(0) != 0x33) )){
         ui->worklineEdit->setText("数据内容错误只能写2/3");
        return;
    }
    writedata.resize(7);
    writedata[5+bytes.size()] = 0;
    writedata[0] = 0xAC;
    writedata[1] = 0x02;
    writedata[2] = 0x0A;
    writedata[3] = 0x01;
    writedata[4] = ( bytes.at(0) - '0');
    writedata[5] = (writedata[1]^writedata[2]^writedata[3]^writedata[4]);
    writedata[6] = 0xCC;
    this->writeData(writedata);
}


void MainWindow::on_readVersionpushButton_clicked()
{
    QByteArray data;
    data.resize(7);
    data[0] = 0xAC;
    data[1] = 0x03;
    data[2] = 0x02;
    data[3] = 0x01;
    data[4] = 0x01;
    data[5] = 0x01;
    data[6] = 0xCC;
    this->writeData(data);
}
