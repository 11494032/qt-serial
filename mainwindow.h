#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "serialthread.h"

class QLabel;
class SettingsDialog;
class Console;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void buttonSendData();
    void  clear();

    void handleError(QSerialPort::SerialPortError error);

    void on_CloselockpushButton_clicked();

    void on_openlockpushButton_clicked();

    void on_QuerypushButton_clicked();

    void on_alarmpushButton_clicked();

    void on_CloseloclearalarmckpushButton_clicked();
    void DisplayMsg(int);
    void handle_data( const QByteArray data);

private:
    void initActionsConnections();

private:
    void showStatusMessage(const QString &message);
    Ui::MainWindow *ui;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
    SerialThread *m_serial_thread = nullptr;
    Console *m_console = nullptr;

};

#endif // MAINWINDOW_H
