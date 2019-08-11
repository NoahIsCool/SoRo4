#-------------------------------------------------
#
# Project created by QtCreator 2018-09-18T08:53:08
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = autonomous
TEMPLATE = lib
DESTDIR = ../../libs

DEFINES += AUTONOMOUS_LIBRARY

QMAKE_CXXFLAGS += -std=c++11

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
INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ -L../../libs -lcore
LIBS += -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_video -lopencv_imgproc -lopencv_videoio

SOURCES += \
    cachetracking.cpp \
    urcauton2020.cpp
    erctraversal.cpp #\
    autonomous.cpp
        SearchAlgorithm.cpp

HEADERS += \
        autonomous_global.h \
    cachetracking.h \
    urcauton2020.h
    erctraversal.h
    autonomous.h
        SearchAlgorithm.h

#unix {
    target.path = $$PWD
    INSTALLS += target
#}
