#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   int pd;
   char buf[32];
   memset(buf,0,sizeof(buf));

  /* pd = ::open("/proc/boardname", O_RDONLY);
   if (::read(pd, buf, 32) < 0){
       QMessageBox::about(this,"error","open boardname failure");
       exit(1);
   }
   if (strncmp(buf,"OK335xD",7) == 0) {
       QMessageBox::about(this,"error","this board doesn't have AD");
       exit(1);
   }

   ::close(pd);
   */
   boardname = buf;

   timer.start(500,this);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    char buf[24];
    int fd;

    memset(buf,0,sizeof(buf));
//    if(boardname == QString("OK335xS"))
    	fd = ::open("/sys/bus/iio/devices/iio:device0/in_voltage4_raw",O_RDONLY);
  //  else if(boardname == QString("OK335xS2"))
    //	fd = ::open("/sys/bus/iio/devices/iio:device0/in_voltage6_raw",O_RDONLY);
    if(fd < 0)
    {
        QMessageBox::about(this,"error","open devices failure");
        exit(-1);
    }

    ::read(fd,buf,sizeof(buf));

    ui->value->setText(QString(buf));

    ::close(fd);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    exit(0);
}

