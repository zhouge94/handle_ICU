#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sys/types.h"
#include"sys/stat.h"
#include"errno.h"
#include"fcntl.h"
#include"stdio.h"
#include"iostream"

#include <sys/ipc.h>
#include <sys/msg.h>
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_sys_param();
    if(sys.planform!=(sys_S::PC)) this->showFullScreen();
    system("rm /tmp/ch*");
    InitWinParam();
    ui->pushButton_param1_5->setFocus();
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
    static QPushButton *bt[9]={ui->pushButton_param1,ui->pushButton_param1_2,ui->pushButton_param1_3,
                       ui->pushButton_param1_4,ui->pushButton_param1_5,ui->pushButton_param1_6,
                       ui->pushButton_param1_7,ui->pushButton_param1_8,ui->pushButton_param1_9};
    static unsigned int forcus=4;
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
        bt[forcus%9]->setFocus();
        break;
    case 8:
        std::cout<<"--"<<std::endl;
        forcus--;
        bt[forcus%9]->setFocus();
        break;
    case 7:
        std::cout<<"ok"<<std::endl;
        bt[forcus%9]->click();
        break;
    case 9:
        std::cout<<"cancel"<<std::endl;
        forcus=4;
        bt[forcus%9]->setFocus();
        break;
    default :break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    QApplication::quit();
}
void MainWindow::on_pushButton_2_clicked()
{
    system("reboot");
}
void MainWindow::on_pushButton_3_clicked()
{
    system("echo  mem>/sys/power/state");
}
void MainWindow::syscmd()
{
    QPushButton *bt[9]={ui->pushButton_param1,ui->pushButton_param1_2,ui->pushButton_param1_3,
                       ui->pushButton_param1_4,ui->pushButton_param1_5,ui->pushButton_param1_6,
                       ui->pushButton_param1_7,ui->pushButton_param1_8,ui->pushButton_param1_9};
    QPushButton *sender_bt=(QPushButton *)sender();
    int param_id;
    for(int i=0;i<9;i++)
    {
        if(sender_bt->objectName()==bt[i]->objectName())
        {
            param_id=i;
            break;
        }
    }
    if(cmd_v.at(param_id).size()>1)
    {
        this->hide();
        system(cmd_v.at(param_id).toStdString().data());
        this->show();
    }
}

void MainWindow::timercallback1()
{
    QLabel *label[9]={ui->label1,ui->label2,ui->label3,
                     ui->label4,ui->label5,ui->label6,
                     ui->label7,ui->label8,ui->label9};
    static  int fd[9];
    char r_buf[50]={0};
    int r_num,err=0,i;
    static int first_in=0;
    const char *fifoname;
    keyEvent();
    if(first_in==0)
    {
        first_in=1;
        for(i=0;i<9;i++)
        {
            if(fifo_name[i].size()>2)
            {
                fifoname=fifo_name[i].toStdString().data();
                if(access(fifoname,F_OK)==-1)
                {
                    if(mkfifo(fifoname,0777)<0&&errno!=EEXIST)
                    {
                        printf("Err while creat fifo[%s]\r\n",fifoname);
                        err=-1;
                    }else
                    {
                        printf("sucess creat fifo[%s]\r\n",fifoname);
                        err=0;
                    }
                }else
                {
                    err=-2;
                    printf("fifo[%s],already exist \r\n",fifoname);
                }
                if(err==0||err==-2)
                {
                    fd[i]=open(fifoname,O_RDONLY|O_NONBLOCK);
                    printf("sucess open fifo [%s] ,id[%d]\r\n",fifoname,fd[i]);
                }
                else
                    fd[i]=0;
            }else
                fd[i]=0;
        }
    }
    for(i=0;i<9;i++)
    {
        if(fd[i]>0)
        {
            r_num=read(fd[i],r_buf,100);
            if(r_num>0)
            {
                printf("CH[%d],r_num[%d],rev[%s]\r\n",i,r_num,r_buf);
                QString *temp=new QString(r_buf);
                label[i]->setText(*temp);
            }
        }
    }
}

void MainWindow::InitWinParam()
{
    QPushButton *bt[9]={ui->pushButton_param1,ui->pushButton_param1_2,ui->pushButton_param1_3,
                       ui->pushButton_param1_4,ui->pushButton_param1_5,ui->pushButton_param1_6,
                       ui->pushButton_param1_7,ui->pushButton_param1_8,ui->pushButton_param1_9};
    my_ini config(CONFIG_FILT);
    char *temp,titile[]="param0";
    int i;
    for(i=0;i<9;i++)
    {
        titile[5]='1'+i;
        temp=config.GetIniKeyString(titile,"title");////////////////////////
        if(temp!=NULL)
        {
             bt[i]->setText(QString(temp));
        }
        temp=config.GetIniKeyString(titile,"action");////////////////////////
        if(temp!=NULL)
        {
             cmd_v.append(QString(temp));
        }else cmd_v.append(" ");
        connect(bt[i],SIGNAL(clicked()),this,SLOT(syscmd()));

        temp=config.GetIniKeyString(titile,"fifo");///////////////////////////
        if(temp!=NULL)
        {
             fifo_name.append(QString(temp));
        }else fifo_name.append(" ");
    }

    QTimer *timer_rev=new QTimer(this);
    connect(timer_rev,SIGNAL(timeout()),this,SLOT(timercallback1()));
    timer_rev->start(100);
    //QTimer *timer_sender=new QTimer(this);
    //connect(timer_sender,SIGNAL(timeout()),this,SLOT(timercallback_sender()));
    //timer_sender->start(100);
}


