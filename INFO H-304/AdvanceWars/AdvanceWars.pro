#-------------------------------------------------
#
# Project created by QtCreator 2018-11-07T18:00:10
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AdvanceWars
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    network.cpp \
    terrain.cpp \
    unit.cpp \
    validmove.cpp \
    airport.cpp \
    airunit.cpp \
    building.cpp \
    city.cpp \
    edge.cpp \
    factory.cpp \
    game.cpp \
    gameobject.cpp \
    infantry.cpp

HEADERS  += mainwindow.h \
    mapsize.h \
    network.h \
    terrain.h \
    unit.h \
    validmove.h \
    airport.h \
    airunit.h \
    building.h \
    city.h \
    edge.h \
    factory.h \
    game.h \
    gameobject.h \
    infantry.h

FORMS    += mainwindow.ui
