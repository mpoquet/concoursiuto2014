#-------------------------------------------------
#
# Project created by QtCreator 2013-03-12T20:28:56
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visu
TEMPLATE = app

LIBS += -lsfml-graphics -lsfml-window -lsfml-system

SOURCES += main.cpp\
    network.cpp \
    netwidget.cpp \
    abstractviewer.cpp \
    graphicsviewer.cpp \
    sfmlviewer.cpp

HEADERS  += \
    network.hpp \
    ../server/struct.hpp \
    ../server/protocole.h \
    netwidget.hpp \
    abstractviewer.hpp \
    graphicsviewer.hpp \
    sfmlviewer.hpp

FORMS += \
    netwidget.ui
