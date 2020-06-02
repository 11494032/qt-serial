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

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void clear();

    void handleError(QSerialPort::SerialPortError error);

    void showClick(QModelIndex index);
    void filterRegExpChanged();
    void filterColumnChanged();
    void sortChanged();

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
};

#endif // MAINWINDOW_H
