#include <sys/ioctl.h>
#include <fcntl.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int pd;
    char buf[32];

   /* pd = open("/proc/boardname", O_RDONLY);

    if (read(pd, buf, 32) < 0){
       perror("read error");
               exit(1);
       }

    if (strncmp(buf,"OK335xS2",8) == 0) {
       QMessageBox::about(this,"error","this board doesn't support audio");
               exit(1);
       }

   ::close(pd);
*/
    ui->setupUi(this);

    playing = false;
    process = NULL;
    process2 = NULL;
    timer = NULL;

    btg = new QButtonGroup;
    btg->addButton(ui->radioMIC,0);
    btg->addButton(ui->radioLINEIN,1);

    ui->slider->setDisabled(true);
    ui->list->addItem(new QListWidgetItem("/forlinx/audio/test.mp3"));
    ui->list->setCurrentRow(0);


}

MainWindow::~MainWindow()
{
    delete ui;
    delete btg;
    delete process;
    delete process2;
}

void MainWindow::on_play_clicked()
{
    if(ui->list->currentItem() == NULL)
    {
        QMessageBox::about(this,"error","nothing selected!");
        return;
    }

    QString file = ui->list->currentItem()->text();

    QFile f(file);

    if(!f.exists())
    {
        QMessageBox::about(this,"error","the file doesn't exist!");
        return;
    }

    QString cmd = QString("/forlinx/bin/mplayer -quiet -slave ")+file;

    if(timer)delete timer;
    if(process)delete process;

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(timerdone()));
    timer->start(1000);  //一秒钟后开始触发，然后一秒一次

    process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(output()));

    process->start(cmd.toAscii());
    process->write("get_time_length \n");

    if(!playing)
    {
        ui->pause->setText("pause");
        ui->slider->setDisabled(false);
    }

    playing = true;

    ui->slider->setMinimum(0);
    ui->slider->setValue(0);
    ui->slider->setDisabled(false);
}

void MainWindow::on_stop_clicked()
{
    if(process == NULL)
        return;

    process->write("stop \n");
    delete process;
    delete timer;
    process = NULL;
    timer = NULL;
}

void MainWindow::on_add_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open file"), " ",  tr("Allfile(*.*);;mp3file(*.mp3);;wmafile(*.wma);;wavefile(*.wav)"));
    ui->list->addItem(new QListWidgetItem(fileName));
    ui->list->setCurrentRow(0);
}

void MainWindow::timerdone()
{
   if(playing)
       process->write("get_time_pos \n");
}

void MainWindow::output()
{

    while(process->canReadLine())
    {
        QString msg(process->readLine());

        if(msg.left(10) == QString("ANS_LENGTH"))
        {
             QString str;
             int len = (int)(msg.right(msg.length()-11).toDouble());
             ui->slider->setMaximum(len);
             str.sprintf("%02d:%02d",len/60,len%60);
             ui->length->setText(str);
        }
        else if(msg.left(17) == QString("ANS_TIME_POSITION"))
        {
             QString str;
             int pos = (int)(msg.right(msg.length()-18).toDouble());
             ui->slider->setValue(pos);
             str.sprintf("%02d:%02d",pos/60,pos%60);
             ui->pos->setText(str);
        }

    }

}


void MainWindow::on_pause_clicked()
{
    if(process == NULL)
        return;

    if(playing)
    {
        ui->pause->setText("resume");
        ui->slider->setDisabled(true);
    }
    else
    {
        ui->pause->setText("pause");
        ui->slider->setDisabled(false);
    }

    process->write("pause \n");

    playing = !playing;

}

void MainWindow::on_slider_sliderMoved(int position)
{
    if(process == NULL)
        return;

    QString tmp = QString("seek ")+QString::number(ui->slider->value())+QString(" 2 \n");

    process->write(tmp.toAscii());
}

void MainWindow::on_del_clicked()
{
    ui->list->removeItemWidget(ui->list->takeItem(ui->list->currentRow()));
}

void MainWindow::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}

void MainWindow::on_record_clicked()
{
    QDateTime dt;
    QDate date;
    QTime time;

    if(process2)
    {
        delete process2;
        process2 = NULL;
    }

    if(ui->record->text() == QString("record"))
    {
        ui->record->setText("stop");

        process2 = new QProcess();

        dt.setDate(date.currentDate());
        dt.setTime(time.currentTime());
        filename = dt.toString("yyyyMMddhhmmss")+QString(".wav");

        if(btg->checkedId() == 0)
            process2->start("arecord -t wav -r 44100 /tmp/"+ filename);
        else
            process2->start("arecord -t wav -c 2 -r 44100 /tmp/"+filename);
    }
    else
    {
         ui->record->setText("record");

         ui->list->addItem(new QListWidgetItem("/tmp/"+filename));
         ui->list->setCurrentRow(ui->list->count()-1);
    }
}
