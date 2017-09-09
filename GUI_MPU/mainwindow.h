#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "ui_mainwindow.h"
#include "qtimer.h"
#include "stdio.h"
#include "unistd.h"
#include "QMessageBox"
#include "sys.h"
#include <qapplication.h>
#include <stdlib.h>
#include "iostream"
#include "filter.h"
#include"qthread.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void OutOneinfo(const QString &msg);
    ~MainWindow();
private slots:
    void UartCallback();
     void realtimeDataSlot_show1();

    void SecondCallBack();
    void onCommTimeout();

private:
    Ui::MainWindow *ui;

    QTimer SecondTimer;
    QTimer dataTimer_show1;
    QTimer UartTimer;
    QTimer CommTimer;

    QThread  uart1Thread;
    QThread  CommThread;

    void setupRealtimeData_show1(QCustomPlot *customPlot);
    void ResetAllData(void);
};
extern QCustomPlot *mbPlot;
#endif // MAINWINDOW_H
