#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
class QLabel;
class SettingsDialog;

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

private:
    void initActionsConnections();

private:
    void showStatusMessage(const QString &message);
    Ui::MainWindow *ui;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
};

#endif // MAINWINDOW_H
