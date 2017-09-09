#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qtimer.h"
#include "stdio.h"
#include "unistd.h"
#include "QMessageBox"
#include "sys.h"
#include "iostream"
#include "filter.h"
extern QApplication *app;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    setupRealtimeData_show1(ui->plot_ecg1);
    setupRealtimeData_show1(ui->plot_ecg2);
    setupRealtimeData_show1(ui->plot_ecg3);
    connect(ui->pushButton_quit,SIGNAL(clicked()),app,SLOT(quit()));

    connect(&SecondTimer, SIGNAL(timeout()), this, SLOT(SecondCallBack()));//the callback for seconds to do sth
    connect(&dataTimer_show1, SIGNAL(timeout()), this, SLOT(realtimeDataSlot_show1()));

/////////////////////////////////////////////////////////////////////////////////////////
    sys.plot_range_TRange_Ecg=5;

    sys.ecgdata_index_cur=0;
    sys.ecgdata_index_last=0;
    sys.ecgrate=250;
 /////////////////////////////////////////////////////////////////////////////////////////
    filter_ecg2.SetFilterA(fir_mb,50);
    filter_ecg1.SetFilterA(fir_mb,50);

    FindMinMax_ecg.Set(-400);
////////////////////////////////////////////////////////////////////////////////////////
    // 使用线程,跑定时器

    CommTimer.start(200);
    CommTimer.setSingleShot(true);
    CommTimer.moveToThread(&CommThread);
    connect(&CommTimer, SIGNAL(timeout()), this, SLOT(onCommTimeout()), Qt::DirectConnection);
    CommThread.start();

    UartTimer.start(200);
    UartTimer.setSingleShot(true);
    UartTimer.moveToThread(&uart1Thread);
    connect(&UartTimer, SIGNAL(timeout()), this, SLOT(UartCallback()), Qt::DirectConnection);
    uart1Thread.start();

    SecondTimer.start(500);
    dataTimer_show1.start(0);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OutOneinfo(const QString &msg)
{
    printf("%s\n",msg.toStdString().data());
    ui->label_info_out->setText(msg);
}
void MainWindow::ResetAllData()
{
    struct  timeval tv;

    sys.ecg1data.clear();
    sys.ecg2data.clear();
    sys.ecgdata_t.clear();
    QVector<double> x, y;
    ui->plot_ecg1->graph()->setData(x, y);
    ui->plot_ecg2->graph()->setData(x, y);
    ui->plot_ecg3->graph()->setData(x, y);

    gettimeofday(&tv,NULL);
    sys.StartTime=tv.tv_sec+tv.tv_usec/1000000.0;

    QDateTime time= QDateTime::currentDateTime();
    QString timstr=time.toString("开始时间：hh:mm:ss");
    //ui->label_start_time->setText(timstr);

}
void MainWindow::setupRealtimeData_show1(QCustomPlot *customPlot)
{

    //customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    QPen pen;

    customPlot->addGraph(); // blue line
    pen.setWidth(2);
    pen.setColor(QColor(Qt::green));
    customPlot->graph(0)->setPen(pen);

    //set some pens, brushes and backgrounds:
     customPlot->yAxis->setBasePen(QPen(Qt::white,1));

     customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    customPlot->yAxis->setTickLabelColor(Qt::white);

    customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));

    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);

    customPlot->setBackground(QColor(10, 10, 10));
    customPlot->xAxis->setTicks(0);
    customPlot->axisRect()->setupFullAxesBox();

/*
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);

    timeTicker->setTimeFormat("%s s");
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1000, 1000);
/*/
    // make left and bottom axes transfer their ranges to right and top axes:
    //connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}


