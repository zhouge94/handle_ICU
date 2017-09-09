#-------------------------------------------------
#
# Project created by QtCreator 2017-03-04T09:17:35
#
#-------------------------------------------------

QT       += core gui
QTPLUGIN += qjpeg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cameraTestQt
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    common.cpp \
    nxp-v4l2.cpp \
    nxp-v4l2-dev.cpp \
    nxp-v4l2-private.cpp \
    camera_thread.cpp \
    v4l2.cpp

HEADERS  += widget.h \
    nx_alloc_mem.h \
    nxp-v4l2.h \
    nxp-v4l2-dev.h \
    camera_thread.h \
    v4l2.h \
    common.h

FORMS    += widget.ui

RESOURCES += \
    img.qrc
