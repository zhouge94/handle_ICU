#include "sys.h"
#include "filter.h"
#include "iostream"
#include "qcustomplot.h"
sys_S sys;
void AddData(unsigned char chx,int *data)
{
    int temp;
    static double ecg_T=1.0/sys.ecgrate;
    static double hx_T=1.0/sys.hxrate;
    if(sys.IsBeginRecode)
    {
        switch(chx)
        {
        case 0:
            temp=data[0];
            //sys.ecg1=filter_ecg1.RealFIR(65536-temp>>16);
            sys.ecg1=temp*1.0;
            sys.ecg1data.append(sys.ecg1);

            temp=data[1];
            //sys.ecg2=filter_ecg2.RealFIR(65536-(temp<<16)>>16);
            sys.ecg2=temp*1.0;
            sys.ecg2data.append(sys.ecg2);

            temp=data[2];
            //sys.ecg2=filter_ecg2.RealFIR(65536-(temp<<16)>>16);
            sys.ecg3=temp*1.0;
            sys.ecg3data.append(sys.ecg3);

            sys.ecgtime+=ecg_T;
            sys.ecgdata_index_cur=sys.ecg1data.count();
            sys.ecgdata_t.append(sys.ecgtime);
            sys.count2++;
            break;
        case 1:
            float temp_float,*p;
            p=(float *)data;
            temp_float=*p;
            temp=data[0];
            sys.hx=temp_float;
            sys.hxdata.append(sys.hx);

            sys.hxtime+=hx_T;
            sys.hxdata_index_cur=sys.hxdata.count();
            sys.hxdata_t.append(sys.hxtime);
            sys.count2++;

            break;
        default:
            break;
        }
    }

}


double GetTimeToDouble(void)
{
    struct  timeval tv;
    double time;
    gettimeofday(&tv,NULL);
    time=tv.tv_sec+tv.tv_usec/1000000.0;
    return time;
}

void SaveData(QString filename)
{
    Sheet ecg_sheet,mpu6050_sheet,common_sheet;
    int i;
    FileParse fd;
    QStringList a;
    system("mkdir Data");
    //save common data
    for(i=0;i<sys.common_t.count();i++)
    {
        a.clear();
        a.append(QString("%1").arg(sys.common_t.at(i),7,'f',6,' '));
        common_sheet.data.append(a);
    }
    fd.generateCSV(QString("Data/")+filename+QString("_common.csv"),common_sheet);

    //tar all the data
    QString cmd=QString("tar -cvf Data/temp.tar Data/*.csv ;rm Data/*.csv;mv Data/temp.tar Data/")+filename+QString(".tar");
    system(cmd.toStdString().c_str());
}
//execute shell command
//执行一个shell命令，输出结果逐行存储在resvec中，并返回行数
int myexec(QString cmd, QList<QString> &resvec) {
    resvec.clear();
    FILE *pp = popen(cmd.toStdString().data(), "r"); //建立管道
    if (!pp) {
        return -1;
    }
    char tmp[1024]; //设置一个合适的长度，以存储每一行输出
    while (fgets(tmp, sizeof(tmp), pp) != NULL) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0'; //去除换行符
        }
        resvec.append(QString(tmp));
    }
    pclose(pp); //关闭管道
    return resvec.count();
}
