#-------------------------------------------------
#
# Project created by QtCreator 2018-09-28T20:41:02
#
#-------------------------------------------------

QT       += widgets

QT       -= gui

TARGET = videoServer
TEMPLATE = lib
DESTDIR = ../../libs

DEFINES += VIDEOSERVER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/..
INCLUDEPATH += /usr/include/gstreamer-1.0 /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include
INCLUDEPATH += /usr/lib/arm-linux-gnueabihf/glib-2.0/include

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -L../../libs -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lcore

SOURCES += \
        videoserver.cpp \
        configreader.cpp \
        socket.cpp \
        crc.cpp

HEADERS += \
        videoserver.h \
        videoserver_global.h \ 
        configreader.h \
        socket.h \
        crc.h

#unix {
    target.path = $$PWD
    INSTALLS += target
#}
