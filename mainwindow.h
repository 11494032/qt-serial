#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QModelIndex>
#include "serialthread.h"
#include <QAbstractItemModel>
#include <QCheckBox>
#include <QComboBox>
#include <QSortFilterProxyModel>
#include <QTreeView>
#include <QLabel>
#include <QGroupBox>

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
    void init();
    void setSourceModel(QAbstractItemModel *model);
    void addSubject(int index,QStandardItemModel *model, const QString &subject);
    void addParamItem(int row, int col,QStandardItemModel *model, const QString &desc);

    void createSubjectModel( QStandardItemModel *model );

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void clear();

    void handleError(QSerialPort::SerialPortError error);

    void showClick(QModelIndex index);
    void wdtPushbutton();
    void spiPushbutton();
    void radarPushbutton();
    void beerPushbutton();
    void iccidPushbutton();
    void macPushbutton();
    void interfaceShow(int i,long data);
    void interfaceStringShow(int i,QString data);

    void on_writeSNPushButton_clicked();

private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);
    Ui::MainWindow *ui;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;
    SerialThread *m_serial_thread = nullptr;
    Console *m_console = nullptr;
    QStringListModel *Model = nullptr;
    QStandardItemModel *ItemModel = nullptr;

    QSortFilterProxyModel *proxyModel;

    QGroupBox *sourceGroupBox;
    QGroupBox *proxyGroupBox;
    QTreeView *sourceView;
    QTreeView *proxyView;
    QCheckBox *filterCaseSensitivityCheckBox;
    QCheckBox *sortCaseSensitivityCheckBox;
    QLabel *filterPatternLabel;
    QLabel *filterSyntaxLabel;
    QLabel *filterColumnLabel;
    QLineEdit *filterPatternLineEdit;
    QComboBox *filterSyntaxComboBox;
    QComboBox *filterColumnComboBox;

    QStandardItemModel *goodsModel;
};

#endif // MAINWINDOW_H
