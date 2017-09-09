#ifndef FILTER_H
#define FILTER_H
#include "qlist.h"
class FIR_Filter
{
public:
    FIR_Filter();
    ~FIR_Filter();
    double RealFIR(float v);
    void SetFilterA(double *AA, int N);
private:
    double *A;
    float V[500];
    int count=0;
    int N;
};
class FindMinMax
{
public:
    FindMinMax();
    ~FindMinMax();
    bool input(double key, float in);
    void Set(double difflevel);
    float level_diff_;
    float level_v_;
    float min;
    float max;
    double minkey;
    double maxkey;
    double DiffMaxKey;
    double DiffMinKey;
    double DiffMinMaxKey;
    double DiffMaxMinKey;
    void Set2(int n0, int n, float Level);
private:
    int index;
    bool IsFindLevel;
    float Diff[2];
    float LastV;
    float Min;
    float Max;
    double LastKey;
    double Lastminkey;
    double Lastmaxkey;
    int count=0;
    int N;
    int N0;
    int MinN;
    int MaxN;
    double MinKey;
    double  MaxKey;

};
extern class FIR_Filter filter_ecg;
extern class FIR_Filter filter_hx;
extern class FIR_Filter filter_ecg2;
extern class FIR_Filter filter_ecg1;

extern class FindMinMax FindMinMax_mb;
extern class FindMinMax FindMinMax_ecg;

extern double fir_mb[50];
extern double fir_ecg[100];
extern double firC[100];
extern double fir_hx[100];
double ZhongZhiFilter(int N0, QList<double>DATE_RAW );

#endif // FILTER_H
