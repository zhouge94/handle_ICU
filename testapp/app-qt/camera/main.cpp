#include "widget.h"
#include <QApplication>
#include "common.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

#ifndef CAMERA_CMD
    w.show();
#endif
    return a.exec();
}
