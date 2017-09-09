#include "my_cvs.h"
bool FileParse::parseCSV(const QString &fileName, Sheet *sheet)
{
    int nameStartIndex = fileName.lastIndexOf('/') + 1;
    if(nameStartIndex < 1)
    {
        nameStartIndex = fileName.lastIndexOf('\\') + 1;
    }
    int nameEndIndex = fileName.lastIndexOf('.');
    sheet->name = fileName.mid(nameStartIndex, nameEndIndex - nameStartIndex);

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return 1;
    }

    QTextStream inStream(&file);

    for( QString lineStr; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if(lineStr.isEmpty())
        {
            continue;
        }

        sheet->data.append(splitCSVLine(lineStr));
    }

    return 0;
}

QStringList FileParse::splitCSVLine(const QString &lineStr)
{
    QStringList strList;
    QString str;

    int length = lineStr.length();
    int quoteCount = 0;
    int repeatQuoteCount = 0;

    for(int i = 0; i < length; ++i)
    {
        if(lineStr[i] != '\"')
        {
            repeatQuoteCount = 0;
            if(lineStr[i] != ',')
            {
                str.append(lineStr[i]);
            }
            else
            {
                if(quoteCount % 2)
                {
                    str.append(',');
                }
                else
                {
                    strList.append(str);
                    quoteCount = 0;
                    str.clear();
                }
            }
        }
        else
        {
            ++quoteCount;
            ++repeatQuoteCount;
            if(repeatQuoteCount == 4)
            {
                str.append('\"');
                repeatQuoteCount = 0;
                quoteCount -= 4;
            }
        }
    }
    strList.append(str);

    return (strList);//    Sheet &operator =(Sheet &&rhs);

}

bool FileParse::generateCSV(const QString &fileName, const Sheet &sheet)
{
    QFile file(fileName);
    bool openOk = file.open(QIODevice::WriteOnly);
    if(!openOk)
    {
        return false;
    }
    QTextStream outStream(&file);

    int strCount = sheet.data.count();
    for(int i = 0; i < strCount; ++i)
    {
        outStream << joinCSVStrs(sheet.data.at(i));
        outStream << '\n';
    }

    return true;
}
QString FileParse::joinCSVStrs(const QStringList &strList)
{
    QString lineStr;

    int strCount = strList.count();
    int lastStrIndex = strCount - 1;

    for(int k = 0; k < strCount; ++k)
    {
        QString tarStr;
        bool commaFlag = false;

        const QString& oriStr = strList.at(k);
        int length = oriStr.length();
        for(int i = 0; i < length; ++i)
        {
            if(oriStr[i] == ',')
            {
                tarStr.append(oriStr[i]);
                commaFlag = true;
            }
            else if(oriStr[i] == '\"')
            {
                tarStr.append("\"\"\"\"");
            }
            else
            {
                tarStr.append(oriStr[i]);
            }
        }
        if(commaFlag)
        {
            tarStr.push_front('\"');
            tarStr.push_back('\"');
        }
        if(k != lastStrIndex)
        {
            tarStr.append(',');
        }

        lineStr.append(tarStr);
    }

    return (lineStr);
}

Sheet::Sheet(){}

