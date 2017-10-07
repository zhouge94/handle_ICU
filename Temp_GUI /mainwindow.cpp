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
float temp_f[5]={11.111,22.222,33.333,44.444,55.555};
unsigned short temp_u16[5];
int temp_index[5]={1,2,3,4,5};
#define FILE_CONF "temp_conf.txt"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init_sys_param();
    if(sys.planform!=(sys_S::PC)) this->showFullScreen();
    system("rm /tmp/ch*");
    InitWinParam();
    ui->pushButton_reset->setFocus();
    update_temp_conf();
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
    static QPushButton *bt[7]={ui->pushButton_t1,ui->pushButton_t2,ui->pushButton_t3,ui->pushButton_t4,ui->pushButton_t5,ui->pushButton_save,ui->pushButton_reset};
    static  unsigned int  forcus=6;
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
        bt[forcus%7]->setFocus();
        break;
    case 8:
        std::cout<<"--"<<std::endl;
        forcus--;
        if(forcus==0)forcus=7;
        bt[forcus%7]->setFocus();
        break;
    case 7:
        std::cout<<"ok"<<std::endl;
        bt[forcus%7]->click();
        break;
    case 9:
        std::cout<<"cancel"<<std::endl;
        forcus=4;
        bt[forcus%7]->setFocus();
        break;
    default :break; 
    }
}
void MainWindow::tempEvent(void)
{
    float *p;
    struct msgstru
    {
            long msg_type;
            unsigned char revbuff[512];
    };
    struct msgstru revs;
    static int msgid,ret_value;
    static int first_in=1,MSGKEY=0x1105;
    if(first_in)
    {
        msgid = msgget(MSGKEY,IPC_EXCL );/*检查消息队列是否存在 */
        if(msgid < 0)  printf("msq not existed! errno=%d [%s] \n\r",errno,strerror(errno));
        else
        {
            do
            {
                ret_value = msgrcv(msgid,&revs,sizeof(temp_f),1,IPC_NOWAIT);
            }while(ret_value>0);
            first_in=0;
        }
    }
    if(!first_in)
    {
        ret_value = msgrcv(msgid,&revs,sizeof(temp_u16),1,IPC_NOWAIT);
        if(ret_value>0)
        {
            memcpy((void *)temp_u16,(void *)revs.revbuff,sizeof(temp_u16));
            for(int i=0;i<5;i++)
            {
               temp_f[i]= temp_u16[i]*1.0/100.0;
            }
        }
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

void MainWindow::timercallback1()
{
    keyEvent();
    tempEvent();
    static int indexsum,indexsum_last;
    indexsum=temp_index[0]+temp_index[1]+temp_index[2]+temp_index[3]+temp_index[4];
    ui->lineEdit_tc1->setText(QString("%1").arg(temp_f[0],0,'g',3));
    ui->lineEdit_tc2->setText(QString("%1").arg(temp_f[1],0,'g',3));
    ui->lineEdit_tc3->setText(QString("%1").arg(temp_f[2],0,'g',3));
    ui->lineEdit_tc4->setText(QString("%1").arg(temp_f[3],0,'g',3));
    ui->lineEdit_tc5->setText(QString("%1").arg(temp_f[4],0,'g',3));

    ui->lineEdit_t1->setText(QString("%1").arg(temp_f[temp_index[0]-1],0,'g',3));
    ui->lineEdit_t2->setText(QString("%1").arg(temp_f[temp_index[1]-1],0,'g',3));
    ui->lineEdit_t3->setText(QString("%1").arg(temp_f[temp_index[2]-1],0,'g',3));
    ui->lineEdit_t4->setText(QString("%1").arg(temp_f[temp_index[3]-1],0,'g',3));
    ui->lineEdit_t5->setText(QString("%1").arg(temp_f[temp_index[4]-1],0,'g',3));
    if(indexsum!=indexsum_last)
    {

        ui->lineEdit_code->setText(QString("%1,%2,%3,%4,%5").arg(temp_index[0]).arg(temp_index[1]).arg(temp_index[2]).arg(temp_index[3]).arg(temp_index[4]));
    }
    indexsum_last=indexsum;
}

void MainWindow::InitWinParam()
{

    QTimer *timer_rev=new QTimer(this);
    connect(timer_rev,SIGNAL(timeout()),this,SLOT(timercallback1()));
    timer_rev->start(100);
    //QTimer *timer_sender=new QTimer(this);
    //connect(timer_sender,SIGNAL(timeout()),this,SLOT(timercallback_sender()));
    //timer_sender->start(100);
}



void MainWindow::on_pushButton_t1_clicked()
{
    QPushButton *p=ui->pushButton_t1;
    QString temp=p->text();
    int index=temp.toInt();
    if(++index>=6)index=1;
    temp_index[0]=index;
    temp=QString("%1").arg(index);
    p->setText(temp);


}

void MainWindow::on_pushButton_t2_clicked()
{
    QPushButton *p=ui->pushButton_t2;
    QString temp=p->text();
    int index=temp.toInt();
    if(++index>=6)index=1;
    temp_index[1]=index;
    temp=QString("%1").arg(index);
    p->setText(temp);

}

void MainWindow::on_pushButton_t3_clicked()
{
    QPushButton *p=ui->pushButton_t3;
    QString temp=p->text();
    int index=temp.toInt();
    if(++index>=6)index=1;
    temp_index[2]=index;
    temp=QString("%1").arg(index);
    p->setText(temp);

}

void MainWindow::on_pushButton_t4_clicked()
{
    QPushButton *p=ui->pushButton_t4;
    QString temp=p->text();
    int index=temp.toInt();
    if(++index>=6)index=1;
    temp_index[3]=index;
    temp=QString("%1").arg(index);
    p->setText(temp);

}

void MainWindow::on_pushButton_t5_clicked()
{
    QPushButton *p=ui->pushButton_t5;
    QString temp=p->text();
    int index=temp.toInt();
    if(++index>=6)index=1;
    temp_index[4]=index;
    temp=QString("%1").arg(index);
    p->setText(temp);

}

void MainWindow::on_pushButton_save_clicked()
{
    FILE *fp;
    char temp[100];
    fp=fopen(FILE_CONF,"wt+");
    if(NULL != fp)
    {
       sprintf(temp,"%s\r\n",ui->lineEdit_code->text().toStdString().data());
       fputs(temp,fp);
       fclose(fp);
    }
    else
    {
        printf("fopen error\r\n");
    }
}
void MainWindow::on_pushButton_reset_clicked()
{
    update_temp_conf();
}
void MainWindow::update_temp_conf()
{

    FILE *fp;
    char temp[100];
    fp=fopen(FILE_CONF,"r");
    if(NULL != fp)
    {
       fgets(temp,10,fp);
       printf("file context[%s]\r\n",temp);
       fclose(fp);
       sscanf(temp,"%d,%d,%d,%d,%d",&temp_index[0],&temp_index[1],&temp_index[2],&temp_index[3],&temp_index[4]);
    }
    else
    {
        printf("file not exist\r\n");
    }
    ui->pushButton_t1->setText(QString("%1").arg(temp_index[0]));
    ui->pushButton_t2->setText(QString("%1").arg(temp_index[1]));
    ui->pushButton_t3->setText(QString("%1").arg(temp_index[2]));
    ui->pushButton_t4->setText(QString("%1").arg(temp_index[3]));
    ui->pushButton_t5->setText(QString("%1").arg(temp_index[4]));

}
