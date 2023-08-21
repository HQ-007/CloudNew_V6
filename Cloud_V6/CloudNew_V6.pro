QT -= gui
QT += sql
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv4 \
               /usr/local/include/opencv2 \

LIBS += /usr/local/lib/libopencv*
LIBS += -lwiringPi \
        -lwiringPiDev \
        -lrt \
        -lm \
        -lcrypt

SOURCES += main.cpp \
    ccustcom.cpp \
    custcamera.cpp \
    custdevicemanagment.cpp \
    custgps.cpp \
    uvc/ircammanager.cpp \
    uvc/v4l2core.c \
    custIrcamera.cpp \
    custtemhum.cpp \
    custclientsocket.cpp \
    custdb.cpp

HEADERS += \
    ccustcom.h \
    custcamera.h \
    custdefine.h \
    custdevicemanagment.h \
    custgps.h \
    uvc/ircammanager.h \
    uvc/v4l2core.h \
    custIrcamera.h \
    custtemhum.h \
    custclientsocket.h \
    custdb.h
