#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
class QLabel;
class settingsDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void Read_Data();

private:
    Ui::MainWindow *ui;
    QLabel *m_status = nullptr;
    settingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
};

#endif // MAINWINDOW_H
