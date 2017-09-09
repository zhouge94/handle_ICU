#ifndef SYS_H
#define SYS_H
#include <QString>
#include "sys/time.h"
#include "my_cvs.h"
#include <iostream>
#include <vector>
#include "string.h"
#include "mapminmax.h"
#include "findpeaks.h"
typedef struct
{
    bool IsBeginRecode;
    float ssxl;
    float pjxl;

    double alltime;
    QList<double> common_t;
    QList<double> rthtdata;
    QList<double> pjhtdata;


    QList<double> ecgdata_t;

     QList<double> ecg1data;
     QList<double> ecg2data;
     QList<double> ecg3data;

    QList<double> maxecg_v;
    QList<double> maxecg_t;
    int ecgdata_index_cur;
    int ecgdata_index_last;
    double ecgrate;
    double ecgtime=0;

    double StartTime;
    double ecg1;
    double ecg2;
    double ecg3;

    int count;
    int count2;
///////////////////////////////////////////////////////////
    double plot_range_TRange_Ecg;
////////////////////////////////////////////////////////////
}sys_S;
extern sys_S sys;

void AddData(unsigned char chx, int *data);
double GetTimeToDouble(void);
void SaveData(QString filename);
int myexec(QString cmd, QList<QString> &resvec);
double XueYang(QList<double> ecg_rr,QList<double> ecg_ri);
#endif
// SYS_H
