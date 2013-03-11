#-------------------------------------------------
#
# Project created by QtCreator 2013-03-09T15:24:50
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = tst_serverreceive
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += ../server/network.hpp \
           ../server/struct.hpp

SOURCES += tst_serverreceive.cpp \
           ../server/network.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
