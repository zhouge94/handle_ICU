#ifndef FINDPEAKS_H
#define FINDPEAKS_H
#include "qlist.h"

class FindPeaks
{
public:
    FindPeaks();
    double MinPeakWidth;
    double MinPeakheigth;
    void Init(double minPeakWidth, double minPeakheigth);
    int Find(QList<double> in, QList<double> in_key, QList<double> *max_key);
};
extern FindPeaks findpeaks;
#endif // FINDPEAKS_H
