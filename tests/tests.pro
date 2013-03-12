#-------------------------------------------------
#
# Project created by QtCreator 2013-03-09T15:24:50
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = testserver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../server/network.hpp \
           ../server/struct.hpp

SOURCES += \
    ../server/network.cpp \
    testserver.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
