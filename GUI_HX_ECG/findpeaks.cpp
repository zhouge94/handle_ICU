#include "findpeaks.h"
#include "qlist.h"
#include "iostream"
using namespace std;
FindPeaks::FindPeaks()
{
    MinPeakWidth=100;
    MinPeakheigth=0;
}
void FindPeaks::Init(double minPeakWidth,double minPeakheigth)
{
    MinPeakWidth=minPeakWidth;
    MinPeakheigth=minPeakheigth;
}
int FindPeaks::Find(QList<double>in,QList<double>in_key,QList<double> *max_key)
{
    QList<int> sign;
    QList<int> indMax;
    int count=in.count();
    for(int i=1;i<count;i++)          /*相邻值做差：小于0，赋-1  *大于0，赋1  *等于0，赋0   */
    {
        double diff = in.at(i) - in.at(i-1);
        if(diff>0)sign.append(1);
        else if(diff<0)sign.append(-1);
        else sign.append(0);
    }
    for(int j=1;j<sign.size();j++)  //再对sign相邻位做差  //保存极大值的位置
    {
        int diff = sign[j]-sign[j-1];
        if(diff<0&&(in[j]>MinPeakheigth)) indMax.push_back(j);
    }

    for(int j=0;j<indMax.count()-1;) //recheck
    {
        if(indMax.at(j+1)-indMax.at(j)<MinPeakWidth)
        {
            if(in.at(indMax[j])>in.at(indMax[j+1]))indMax.removeAt(j+1);
            else indMax.removeAt(j);
        }else
        {
            j++;
        }
    }
    //cout<<"极大值为:"<<endl;
    for(int m = 0;m<indMax.size();m++)
    {
        max_key->append(in_key.at(indMax[m]));
        //cout<<"num["<<m<<"]="<<in.at((indMax[m]))<<" key: "<<max_key->last()<<"  ";
    }
    cout<<endl;
}
FindPeaks findpeaks;
