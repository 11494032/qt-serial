#include "mainwindow.h"
#include <QApplication>
#include <QStandardItemModel>
#include <QTime>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("地锁功能测试");
 //   w.setSourceModel(createMailModel(&w));
    w.show();

    return a.exec();
}
