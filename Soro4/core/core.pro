#-------------------------------------------------
#
# Project created by QtCreator 2018-09-30T17:35:34
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = core
TEMPLATE = lib
DESTDIR = ../../libs

DEFINES += CORE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/gstreamer-1.0 /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include
INCLUDEPATH += /usr/lib/arm-linux-gnueabihf/glib-2.0/include

LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0

SOURCES += \
    #gps/gps.cpp \
    #gps/gps2.cpp \
    #gps/main.cpp \
    #gps/tcp_example2.cpp \
    gps/edc.c \
    gps/gps.c \
    #gps/main.c \
    gps/sbp.c \
    #gps/tcp_example2.c
    comms.cpp


HEADERS += \
        core.h \
	comms.h \
        gps/acquisition.h \
        gps/bootload.h \
        gps/common.h \
        gps/edc.h \
        gps/ext_events.h \
        gps/file_io.h \
        gps/flash.h \
        gps/gnss.h \
        gps/gps.h \
        #gps/gps.hpp \
        #gps/gps2.hpp \
        gps/imu.h \
        gps/logging.h \
        gps/mag.h \
        gps/navigation.h \
        gps/ndb.h \
        gps/observation.h \
        gps/orientation.h \
        gps/piksi.h \
        gps/sbas.h \
        gps/sbp.h \
        gps/settings.h \
        gps/ssr.h \
        gps/system.h \
        gps/tracking.h \
        gps/user.h \
        gps/vehicle.h \
        gps/version.h \
    comms.h \
    videostreamer_global.h

#unix {
    target.path = $$PWD/
    INSTALLS += target
#}
