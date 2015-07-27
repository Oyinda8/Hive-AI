#-------------------------------------------------
#
# Project created by QtCreator 2015-07-03T23:43:33
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HexGame1
TEMPLATE = app


SOURCES += main.cpp \
    game.cpp \
    tile.cpp \
    miscCode.cpp

HEADERS  += \
    game.h \
    tile.h

FORMS    +=

RESOURCES += \
    Resource.qrc
