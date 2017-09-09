TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    nxp-v4l2.cpp \
    nxp-v4l2-dev.cpp \
    common.cpp \
    jpeg.cpp



include(deployment.pri)
qtcAddDeployment()

LIBS += -L. -ljpeg

HEADERS += \
    nx_alloc_mem.h \
    nxp-v4l2.h \
    nxp-v4l2-dev.h \
    jpeglib.h

