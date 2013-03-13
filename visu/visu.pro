#-------------------------------------------------
#
# Project created by QtCreator 2013-03-12T20:28:56
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visu
TEMPLATE = app


SOURCES += main.cpp\
    network.cpp \
    netwidget.cpp

HEADERS  += \
    network.hpp \
    ../server/struct.hpp \
    ../server/protocole.h \
    netwidget.hpp

FORMS += \
    netwidget.ui
