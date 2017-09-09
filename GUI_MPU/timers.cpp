#include "mainwindow.h"

#include "sys/types.h"
#include"sys/stat.h"
#include"errno.h"
#include"fcntl.h"


#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGKEY 0x1102
struct ECG_S
{
    short int ecg1;
    short int ecg2;
    short int ecg3;
};
struct ECG_HR_S
{
    float rthr;
    float avehr;
};
struct msgstru
{
        long msg_type;
        unsigned char revbuff[512];
};
struct msgstru revs;
struct ECG_S ecgs;
void MainWindow::UartCallback()
{
    static int msgid,ret_value,firstin=1;
    static int ecgchx[3]={0};
    if(firstin)
    {
        msgid = msgget(MSGKEY,IPC_EXCL );/*检查消息队列是否存在 */
        if(msgid < 0)
        {
            sys.IsBeginRecode=0;
            printf("msq not existed! errno=%d [%s] \n\r",errno,strerror(errno));
            usleep(1000000);
        }else
        {
            firstin=0;
            sys.IsBeginRecode=1;
            do
            {
             ret_value = msgrcv(msgid,&revs,sizeof(ecgs),1,IPC_NOWAIT);
            }while(ret_value>0);

        }
    } else
    {
        ret_value = msgrcv(msgid,&revs,sizeof(ecgs),1,0);
        if(ret_value>0)
        {
            memcpy(&ecgs,&revs.revbuff,sizeof(ecgs));
            ecgchx[0]=ecgs.ecg1;
            ecgchx[1]=ecgs.ecg2;
            ecgchx[2]=ecgs.ecg3;
            AddData(0,ecgchx);
            sys.count++;
        }else
        {
            usleep(1000);
        }
    }
    UartTimer.start(0);
}
void MainWindow::realtimeDataSlot_show1()
{
    static double ecg_key;
    static int FirstIn=0;

    QVector<double> vector_ecgkey;
    QVector<double> vector_ecg1data;
    QVector<double> vector_ecg2data;
    QVector<double> vector_ecg3data;
    static QCPItemTracer *ecg1Tracer = new QCPItemTracer(ui->plot_ecg1);
    static QCPItemTracer *ecg2Tracer = new QCPItemTracer(ui->plot_ecg2);
    static QCPItemTracer *ecg3Tracer = new QCPItemTracer(ui->plot_ecg3);

    static double StartT_ecg;

    int i,i2,i3;
    if(FirstIn==0)
    {
        ecg1Tracer->setGraph(ui->plot_ecg1->graph(0));
        ecg1Tracer->setInterpolating(true);
        ecg1Tracer->setStyle(QCPItemTracer::tsCircle);
        ecg1Tracer->setPen(QPen(Qt::red));
        ecg1Tracer->setBrush(Qt::red);
        ecg1Tracer->setSize(10);

        ecg2Tracer->setGraph(ui->plot_ecg2->graph(0));
        ecg2Tracer->setInterpolating(true);
        ecg2Tracer->setStyle(QCPItemTracer::tsCircle);
        ecg2Tracer->setPen(QPen(Qt::red));
        ecg2Tracer->setBrush(Qt::red);
        ecg2Tracer->setSize(10);

        ecg3Tracer->setGraph(ui->plot_ecg3->graph(0));
        ecg3Tracer->setInterpolating(true);
        ecg3Tracer->setStyle(QCPItemTracer::tsCircle);
        ecg3Tracer->setPen(QPen(Qt::red));
        ecg3Tracer->setBrush(Qt::red);
        ecg3Tracer->setSize(10);

        ui->plot_ecg1->yAxis->setRange(-100000,100000);
        ui->plot_ecg2->yAxis->setRange(-100000,100000);
        ui->plot_ecg3->yAxis->setRange(-100000,100000);
        ui->plot_ecg1->xAxis->setRange(0,sys.plot_range_TRange_Ecg);
        ui->plot_ecg2->xAxis->setRange(0,sys.plot_range_TRange_Ecg);
        ui->plot_ecg3->xAxis->setRange(0,sys.plot_range_TRange_Ecg);

        FirstIn++;
        StartT_ecg=0;
    }else
    {
        if(sys.IsBeginRecode)
        {
            static QTime time(QTime::currentTime());    // calculate two new data points:
            double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
            static double lastPointKey = 0;

            if (key-lastPointKey > 0.05) // at most add point every 20 ms
            {
                i2=sys.ecgdata_index_last;
                i3=sys.ecgdata_index_cur;
                sys.ecgdata_index_last=i3;
                for(i=i2;i<i3;i++)//copy the data to the show buff
                {
                    if(i%3==0)
                    {
                        ecg_key=sys.ecgdata_t.at(i);
                        vector_ecgkey.append(ecg_key-StartT_ecg);
                        vector_ecg1data.append(sys.ecg1data.at(i));
                        vector_ecg2data.append(sys.ecg2data.at(i));
                        vector_ecg3data.append(sys.ecg3data.at(i));
                    }
                }
                if(vector_ecgkey.count()>0)
                {
                    ui->plot_ecg1->graph(0)->data().data()->remove(vector_ecgkey.first(),vector_ecgkey.last()+0.05);
                    ui->plot_ecg1->graph(0)->addData(vector_ecgkey,vector_ecg1data);
                    ui->plot_ecg2->graph(0)->data().data()->remove(vector_ecgkey.first(),vector_ecgkey.last()+0.05);
                    ui->plot_ecg2->graph(0)->addData(vector_ecgkey,vector_ecg2data);
                    ui->plot_ecg3->graph(0)->data().data()->remove(vector_ecgkey.first(),vector_ecgkey.last()+0.05);
                    ui->plot_ecg3->graph(0)->addData(vector_ecgkey,vector_ecg3data);
                    ecg1Tracer->setGraphKey(vector_ecgkey.last());
                    ecg2Tracer->setGraphKey(vector_ecgkey.last());
                    ecg3Tracer->setGraphKey(vector_ecgkey.last());
                }
                lastPointKey = key;
                ui->plot_ecg1->replot();
                ui->plot_ecg2->replot();
                ui->plot_ecg3->replot();

                static double LastecgPointKey=0;
                if( (ecg_key-LastecgPointKey > sys.plot_range_TRange_Ecg))
                {
                    LastecgPointKey=ecg_key;
                    QList<double>ecg_t_temp,ecg_d_temp,ecg_d_temp_1,ecg_d2_temp,ecg_d2_temp_1,ecg_d3_temp,ecg_d3_temp_1;
                    if(vector_ecgkey.count()>0)
                    {
                        StartT_ecg=ecg_key;
                        ui->plot_ecg1->graph(0)->data().data()->removeAfter(sys.plot_range_TRange_Ecg);
                        ui->plot_ecg2->graph(0)->data().data()->removeAfter(sys.plot_range_TRange_Ecg);
                        ui->plot_ecg3->graph(0)->data().data()->removeAfter(sys.plot_range_TRange_Ecg);
                        int count=(ui->plot_ecg1->graph(0)->data().data()->size());
                        for(i=10;i<count-10;i++)
                        {
                            ecg_t_temp.append(ui->plot_ecg1->graph(0)->data().data()->at(i)->key);
                            ecg_d_temp.append(ui->plot_ecg1->graph(0)->data().data()->at(i)->value);
                            ecg_d2_temp.append(ui->plot_ecg2->graph(0)->data().data()->at(i)->value);
                            ecg_d3_temp.append(ui->plot_ecg3->graph(0)->data().data()->at(i)->value);
                        }
                        MapMinMax *mapminmax_t=new MapMinMax;
                        MapMinMax *mapminmax_t2=new MapMinMax;
                        MapMinMax *mapminmax_t3=new MapMinMax;
                        mapminmax_t->GetMinMax(ecg_d_temp);
                        mapminmax_t->Change(ecg_d_temp,&ecg_d_temp_1);
                        mapminmax_t2->GetMinMax(ecg_d2_temp);
                        mapminmax_t2->Change(ecg_d2_temp,&ecg_d2_temp_1);
                        mapminmax_t3->GetMinMax(ecg_d3_temp);
                        mapminmax_t3->Change(ecg_d3_temp,&ecg_d3_temp_1);
                        {
                            if(mapminmax_t->xmax==0&&mapminmax_t->xmin==0)ui->plot_ecg1->yAxis->setRange(-10000,10000);
                            else ui->plot_ecg1->yAxis->setRange(mapminmax_t->xmin-mapminmax_t->xrange*0.5,mapminmax_t->xmax+mapminmax_t->xrange*0.5);
                        }
                        {
                            if(mapminmax_t2->xmax==0&&mapminmax_t2->xmin==0)ui->plot_ecg2->yAxis->setRange(-10000,10000);
                             else ui->plot_ecg2->yAxis->setRange(mapminmax_t2->xmin-mapminmax_t2->xrange*0.5,mapminmax_t2->xmax+mapminmax_t2->xrange*0.5);
                        }
                        {
                            if(mapminmax_t3->xmax==0&&mapminmax_t3->xmin==0)ui->plot_ecg3->yAxis->setRange(-10000,10000);
                             else ui->plot_ecg3->yAxis->setRange(mapminmax_t3->xmin-mapminmax_t3->xrange*0.5,mapminmax_t3->xmax+mapminmax_t3->xrange*0.5);
                        }

                    }

                    ui->plot_ecg1->xAxis->setRange(0,sys.plot_range_TRange_Ecg);
                    ui->plot_ecg2->xAxis->setRange(0,sys.plot_range_TRange_Ecg);
                    ui->plot_ecg3->xAxis->setRange(0,sys.plot_range_TRange_Ecg);
                    ui->plot_ecg1->replot();
                    ui->plot_ecg2->replot();
                    ui->plot_ecg3->replot();
                }

            }
        }
    }
}

void MainWindow::SecondCallBack()
{
    static int count,pjxl_sum,i;
    static int FirstIn=0;
    if(FirstIn==0)
    {
        FirstIn=1;
        //this->on_pushButton_test_clicked();
#ifndef QT_DEBUG
        this->showFullScreen();
#endif
    }
    if(count++<20)pjxl_sum+=sys.ssxl;
    else
    {
        sys.pjxl=pjxl_sum/20.0;
        count=0;
        pjxl_sum=0;
    }
    if(i++%2)
    {
        //std::cout<<"rate1:"<<sys.count<<",rate1:"<<sys.count2<<std::endl;
        OutOneinfo((QString("%1 FPS").arg(sys.count,5,'f',1,' ')));
        sys.count=0;
        sys.count2=0;
    }
    ui->show1_ssxl->setText(QString("%1").arg(sys.ssxl,5,'f',1,' '));

}

void MainWindow::onCommTimeout()
{
    double temp;
    QDateTime time= QDateTime::currentDateTime();
    QString timstr=time.toString("yyyy-MM-dd hh:mm:ss");
    //ui->label_current_time->setText(timstr);
    CommTimer.start(100);
    if(sys.IsBeginRecode)
    {
        temp=sys.ecgtime;
        sys.common_t.append(temp);
        sys.rthtdata.append(sys.ssxl);
        sys.pjhtdata.append(sys.pjxl);
        //update common data
    }
}
