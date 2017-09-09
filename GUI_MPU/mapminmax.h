#ifndef MAPMINMAX_H
#define MAPMINMAX_H
#include "qlist.h"

class MapMinMax
{
public:
    MapMinMax();
    double xmin,xmax,xrange,gain,xoffset;
    double sum,averge;
    void Change(QList<double> x, QList<double> *y);
    void GetMinMax(QList<double> x);
};
extern MapMinMax mapminmax;

#endif // MAPMINMAX_H
