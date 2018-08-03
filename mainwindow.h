#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initPort();


private slots:
    void on_btnOpen_clicked();

    void on_btnSend_clicked();
    void Read_Data();

private:
    Ui::MainWindow *ui;
    QSerialPort *my_serialport;
};

#endif // MAINWINDOW_H
