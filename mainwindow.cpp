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

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    ui->statusBar->addWidget(m_status);
    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this,&MainWindow::readData);
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
    init();
}


void MainWindow::init()
{

     /*
        ItemModel = new QStandardItemModel(this);

          QStringList strList;
          strList.append("A");
          strList.append("B");
          strList.append("C");
          strList.append("D");
          strList.append("E");
          strList.append("F");
          strList.append("G");

          int nCount = strList.size();
          for(int i = 0; i < nCount; i++)
          {
              QString string = static_cast<QString>(strList.at(i));
              QStandardItem *item = new QStandardItem(string);
              ItemModel->appendRow(item);
          }
         ui->listView->setModel(ItemModel);
         ui->listView->setFixedSize(200,300);

         connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(showClick(QModelIndex)));
    */

    ui->treeView->setRootIsDecorated(false);
    ui->treeView->setAlternatingRowColors(true);
    QStandardItemModel *goodsModel = new QStandardItemModel(0, 4,this);
    ui->treeView->setColumnWidth(0,50);
    ui->treeView->setColumnWidth(1,400);
    ui->treeView->setColumnWidth(2,400);
    ui->treeView->setColumnWidth(3,400);
  /*  goodsModel->setHeaderData(0, Qt::Horizontal, tr("No"));
    goodsModel->setHeaderData(1, Qt::Horizontal, tr("测试项"));
    goodsModel->setHeaderData(2, Qt::Horizontal, tr("测试结果"));
    goodsModel->setHeaderData(3, Qt::Horizontal, tr("参考"));
    */
    ui->treeView->setModel(goodsModel);
    createSubjectModel(goodsModel);
/*
    for (int i = 0; i < 4; ++i){
        QList<QStandardItem *> items;
        for (int i = 0; i < 3; ++i) {
            QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
            if (0 == i)
                item->setCheckable(true);
            items.push_back(item);
        }
        goodsModel->appendRow(items);
        for (int i = 0; i < 4; ++i)
        {
            QList<QStandardItem *> childItems;
            for (int i = 0; i < 3; ++i){
                QStandardItem *item = new QStandardItem(QString("%0").arg(i));
                if (0 == i)
                    item->setCheckable(true);
                childItems.push_back(item);
            }
            items.at(0)->appendRow(childItems);
        }
    }
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    goodsModel->setData(goodsModel->index(3,3),"成功");
    */
}
void MainWindow::addSubject(int index,QStandardItemModel *model, const QString &subject)
{
    model->insertRow(0);
    model->setData(model->index(0, 0), index);
    model->setData(model->index(0, 1), subject);
}

void MainWindow:: createSubjectModel( QStandardItemModel *model )
{
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("序号"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("测试项"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("测试结果"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("参考"));
    addSubject(1,model, "spiflash" );
    addSubject(0,model, "看门狗" );

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



