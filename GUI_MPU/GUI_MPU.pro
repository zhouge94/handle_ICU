#-------------------------------------------------
#
# Project created by QtCreator 2016-12-06T13:25:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
TARGET = MPU_GUI
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    qcustomplot.cpp \
    sys.cpp \
    filter.cpp \
    timers.cpp \
    my_cvs.cpp \
    mapminmax.cpp \
    findpeaks.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    my_uart.h \
    sys.h \
    filter.h \
    my_cvs.h \
    mapminmax.h \
    findpeaks.h \
    my_ini.h

FORMS    += mainwindow.ui
DEFINES += QT_NO_DEBUG_OUTPUT
