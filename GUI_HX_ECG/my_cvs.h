#ifndef MY_CVS_H
#define MY_CVS_H
#include <QString>
#include <QStringList>
#include "QFile"
#include "QTextStream"
class Sheet
{
    enum FieldType{STRING, INT, DOUBLE, BOOL};
public:
    Sheet();
public:
    QString name;
    QList<QStringList> data;
    QList<FieldType> fieldTypes;
};

class FileParse
{
public:
    bool generateCSV(const QString &fileName, const Sheet &sheet);
    bool parseCSV(const QString &fileName, Sheet *sheet);
private:
    QString joinCSVStrs(const QStringList &strList);
    QStringList splitCSVLine(const QString &lineStr);
};
#endif // MY_CVS_H
