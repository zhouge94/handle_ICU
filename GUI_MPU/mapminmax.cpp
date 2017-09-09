#include "mapminmax.h"
MapMinMax mapminmax;

MapMinMax::MapMinMax()
{

}

void MapMinMax::Change(QList<double> x,QList<double> *y)
{
    int  i;
    for(i=0;i<x.count();i++)
    {
        y->append((x.at(i)-xoffset)*gain);
    }
}

void MapMinMax::GetMinMax(QList<double> x)
{
    int i;
    xmax=x.at(0);
    xmin=x.at(0);
    sum=0;
    for(i=1;i<x.count();i++)
    {
        if(x.at(i)>xmax)xmax=x.at(i);
        else if(x.at(i)<xmin)xmin=x.at(i);
        sum+=x.at(i);
    }
    xrange=xmax-xmin;
    averge=sum/(x.count()*1.0);
    xoffset=(xmin+xmax)/2;
    gain=2/xrange;
}
