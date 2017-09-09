#include "mainwindow.h"
#include <QtCore>
#include <QApplication>
QApplication *app;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    app=&a;
#ifndef QT_DEBUG
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));//tr使用的编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));//QString使用的编码，没有这一条
#endif
    MainWindow *ecg_icu;
    ecg_icu=new MainWindow;
    ecg_icu->show();
    std::cout<<"begin to run core"<<std::endl;
    return a.exec();
}

