
#include "gprs.h"
#include "ui_gprs.h"
#include"stdio.h"
#include <fcntl.h>
#include <qmessagebox.h>
#include <termio.h>
#include <unistd.h>


int m_fd=-1;


Gprs::Gprs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Gprs)
{
    ui->setupUi(this);

    myprocess = NULL;
    m_startgprs = new QProcess;
    connect(m_startgprs, SIGNAL(readyRead()), this, SLOT(readOutput()));
}

Gprs::~Gprs()
{
    ::system("killall pppd");

    if (m_fd >= 0)
    {
       ::close(m_fd);
       m_fd = -1;
     }
    delete ui;

    if(myprocess)
        delete myprocess;
}

void Gprs::on_pbt_set_clicked()
{
    if(ui->pbt_set->text()=="disconnect")
    {
        if (m_fd >= 0)
        {
           ::close(m_fd);
           m_fd = -1;
         }
        ui->pbt_set->setText("set");
        return;
    }
    m_fd = openSerialPort();
    if (m_fd < 0)
    {
        QMessageBox::warning(this, tr("Error"), tr("Fail to open serial port!"));
        return ;
    }
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect (m_notifier, SIGNAL(activated(int)), this, SLOT(remoteDataIncoming()));
    ui->pbt_set->setText("disconnect");
    ::write(m_fd,"AT+CNMI=2,1,0,0,0\r",strlen("AT+CNMI=2,1,0,0,0\r"));
    sleep(1);
}

void Gprs::result()
{
    QString abc = myprocess->readAllStandardOutput();
    ui->Edit_info->append(abc.trimmed());
    QString efg = myprocess->readAllStandardError();
    if(efg.length()>1)ui->Edit_info->append(efg.trimmed());
}


void Gprs::on_pbt_call_clicked()
{
    if (m_fd < 0)
    {
        QMessageBox::warning(this, tr("Error"), tr("Fail to set serial port!"));
        return ;
    }
    if(ui->pbt_call->text()=="stop")
    {
       ::write(m_fd,"ATH\r",strlen("ATH\r"));
        sleep(1);
        ui->Edit_info->clear();
        ui->pbt_call->setText("call");
        return;

    }
    if (ui->Edit_phone->text().length() < 11)
    {
        ui->Edit_info->setText("输入的电话号码错误。\n");
    }
    else
    {
        telnum=ui->Edit_phone->text();
        if (telnum.length()!=11)
        {
             QMessageBox::warning(this, tr("warming"), tr("phono number set error!"));
        }
        QString send="ATD"+telnum+";\r";

       if(::write(m_fd, send.toAscii(), send.length()))
       {
           ui->pbt_call->setText("stop");
           ui->Edit_info->setText("calling "+telnum);

       }

    }
}

void Gprs::on_pbt_send_clicked()
{
    if (m_fd < 0)
    {
        QMessageBox::warning(this, tr("Error"), tr("Fail to set serial port!"));
        return ;
    }

    QString mess=ui->Line_msg->text()+"\x1a";

    QString tell = "AT+CMGS=\"" +  ui->Edit_phone->text() + "\"\r";

    if (ui->Edit_phone->text().length()!=11)
    {
         QMessageBox::warning(this, tr("warming"), tr("phono number set error!"));
    }
    char getch[15];

    if(::write(m_fd,"AT+CSCS=\"GSM\"\r",strlen("AT+CSCS=\"GSM\"\r")))
    {
        sleep(1);

    }
    if(::write(m_fd,tell.toAscii(),tell.length()))
    {
        sleep(1);

    }

    if(::write(m_fd,mess.toAscii(),mess.length()))
    {
        sleep(3);
       // QMessageBox::information(this,"gprs","send message OK",0);
    }

    if(::read(m_fd,getch,strlen(getch)))
    {
        sleep(1);
       // QMessageBox::information(this,tr(""),tr(getch));
    }
}


void Gprs::on_pbt_gprs_clicked()
{
    if(ui->pbt_gprs->text()=="stop")
    {
       ::system("killall pppd");
        ui->pbt_gprs->setText("gprs");
        return;
    }
    ::system("ifconfig eth0 down");
    ::system("ifconfig eth1 down");
    ::system("rm /etc/resolv.conf");
    ::system("ln -s /etc/ppp/resolv.conf /etc/resolv.conf");

     m_startgprs->terminate();

     QString dialcmd= "pppd call gprs /dev/";

     dialcmd+=ui->cb_com->currentText();

     m_startgprs->start(dialcmd);

     ui->pbt_gprs->setText("stop");

}

int Gprs::openSerialPort()
{
  int fd = -1;

  portName="/dev/"+ui->cb_com->currentText();
  fd = ::open(portName.toStdString().data(), O_RDWR|O_NONBLOCK);
  if (fd < 0)
  {
    QMessageBox::warning(this, tr("Error"), tr("fd1"));
    return -1;
  }

  termios serialAttr;
  memset(&serialAttr, 0, sizeof serialAttr);
/*
serialAttr.c_iflag = IGNPAR;
  serialAttr.c_cflag = B115200 | HUPCL | CS8 | CREAD | CLOCAL;
  serialAttr.c_cc[VMIN] = 1;
*/
if(ui->cb_bound->currentText() == tr("9600"))
    {
        cfsetispeed(&serialAttr,B9600);
        cfsetospeed(&serialAttr,B9600);
    }
     else if(ui->cb_bound->currentText() == tr("115200"))
    {
        cfsetispeed(&serialAttr,B9600);
        cfsetospeed(&serialAttr,B115200);
    }
     if(ui->cb_data->currentText() == tr("8"))
    {
        serialAttr.c_cflag &= ~CSIZE;
        serialAttr.c_cflag |= CS8;
    }
     else if(ui->cb_data->currentText() == tr("7"))
    {
        serialAttr.c_cflag &= ~CSIZE;
        serialAttr.c_cflag |= CS7;
    }
    if(ui->cb_partity->currentText() == tr("No"))
        serialAttr.c_iflag = IGNPAR;
    else if(ui->cb_partity->currentText() == tr("Odd"))
    {
        serialAttr.c_cflag |= (PARODD | PARENB);
        serialAttr.c_iflag |= INPCK;
    }
    else if(ui->cb_partity->currentText() == tr("Even"))
    {
        serialAttr.c_cflag |= PARENB;
        serialAttr.c_cflag &= ~PARODD;
        serialAttr.c_iflag |= INPCK;
    }
  if (tcsetattr(fd, TCSANOW, &serialAttr) != 0)
  {
    QMessageBox::warning(this, tr("Error"), tr("fd2"));
    return -1;
  }
  return fd;
}



void Gprs::remoteDataIncoming()
{
    char getch[30];
    if(::read(m_fd,getch,30))
    {
        sleep(1);  
        QString mes_or_tell = QString(getch);

        int tell_iscome=mes_or_tell.indexOf(QRegExp("RING"), 0);
        int mes_iscome=mes_or_tell.indexOf(QRegExp("CMTI"), 0);
        if(mes_iscome>=0)
        {
          QMessageBox::information(this,"gprs","message is received",0);
        }
        if(tell_iscome>=0)
        {
           QMessageBox::information(this,"gprs","tell is coming",0);
        }
    }
}



void Gprs::readOutput()
{
    while (m_startgprs->canReadLine())
     {
        QString gprsInfo = m_startgprs->readAll();
        ui->Edit_info->append(gprsInfo);
    }

}

void Gprs::closeEvent(QCloseEvent *)
{
    destroy();
    exit(0);
}

void Gprs::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void Gprs::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}


void Gprs::on_ping_clicked()
{
    if(ui->hostname->text() == QString(""))
    {
        QMessageBox::about(this,"error","hostname cannot be empty!");
        return;
    }

    if(myprocess)
        delete myprocess;

    myprocess = new QProcess(this);
    connect(myprocess, SIGNAL(readyReadStandardOutput()),this, SLOT(result()));
    connect(myprocess, SIGNAL(readyReadStandardError()),this, SLOT(result()));
    myprocess->start(QString("ping -c 5 ")+ui->hostname->text());
    ui->Edit_info->append(QString("ping -c 5 ")+ui->hostname->text());
}
