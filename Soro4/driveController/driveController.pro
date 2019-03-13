#-------------------------------------------------
#
# Project created by QtCreator 2018-09-18T09:57:36
#
#-------------------------------------------------

QT       += network gui widgets gamepad

TARGET = driveController
TEMPLATE = lib
DESTDIR = ../../libs

DEFINES += DRIVECONTROLLER_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/..
LIBS = -L../../libs -lcore -lpthread

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
        drivecontroller.cpp \
    gamepadmonitor.cpp

HEADERS += \
        drivecontroller.h \
        drivecontroller_global.h \
    gamepadmonitor.h

#unix {
    target.path = $$PWD/
    INSTALLS += target
#}

FORMS += \
    drivecontroller.ui
