#-------------------------------------------------
#
# Project created by QtCreator 2013-08-30T15:04:12
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TuxIMServer
TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    db.cpp \
    config.cpp

HEADERS  += \
    server.h \
    db.h \
    config.h

FORMS    +=
