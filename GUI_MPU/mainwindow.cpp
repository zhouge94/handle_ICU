#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qtimer.h"
#include "stdio.h"
#include "unistd.h"
#include "QMessageBox"
#include "sys.h"
#include "iostream"
#include "filter.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include "sys/types.h"
#include"sys/stat.h"
#include"errno.h"
#include"fcntl.h"
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
    QTimer *timer_key=new QTimer;
    connect(timer_key,SIGNAL(timeout()),this,SLOT(keyEvent()));
    timer_key->start(100);

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::keyEvent(void)
{
    struct msgstru
    {
            long msg_type;
            unsigned char revbuff[512];
    };
    struct msgstru revs;
    static int msgid,ret_value;
    static int first_in=1,MSGKEY=0x1100;
//    static QPushButton *bt[9]={ui->pushButton_param1,ui->pushButton_param1_2,ui->pushButton_param1_3,
//                       ui->pushButton_param1_4,ui->pushButton_param1_5,ui->pushButton_param1_6,
//                       ui->pushButton_param1_7,ui->pushButton_param1_8,ui->pushButton_param1_9};
    static unsigned int forcus=4,exit_n=0;
    int key=0;
    if(first_in)
    {
        msgid = msgget(MSGKEY,IPC_EXCL );/*检查消息队列是否存在 */
        if(msgid < 0)  printf("msq not existed! errno=%d [%s] \n\r",errno,strerror(errno));
        else
        {
            do
            {
                ret_value = msgrcv(msgid,&revs,1,1,IPC_NOWAIT);
            }while(ret_value>0);
            first_in=0;
        }
    }
    if(!first_in)
    {
        ret_value = msgrcv(msgid,&revs,1,1,IPC_NOWAIT);
        if(ret_value>0)
        {
            key=revs.revbuff[0];
        }else
        {
           key=0;
        }
    }
    switch(key)
    {
    case 2:
        std::cout<<"++"<<std::endl;
        forcus++;
        //bt[forcus%9]->setFocus();
        break;
    case 8:
        std::cout<<"--"<<std::endl;
        forcus--;
        //bt[forcus%9]->setFocus();
        break;
    case 7:
        std::cout<<"ok"<<std::endl;
        if(exit_n==1)
        {
            app->exit();
        }
        break;
        //bt[forcus%9]->click();
    case 9:
        std::cout<<"exit"<<std::endl;
        if(exit_n==0)
        {
            exit_n=1;
            OutOneinfo(QString("退出?"));
        }else
        {
            exit_n=0;
        }
        //forcus=4;
        //bt[forcus%9]->setFocus();
        break;
    default :break;
    }
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


