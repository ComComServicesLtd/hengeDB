#-------------------------------------------------
#
# Project created by QtCreator 2015-08-28T16:50:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

CONFIG += c++11

TARGET = bstore
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    timeseries.cpp \
    indexmanager.cpp \
    seriesmanager.cpp \
    seriesindex.cpp \
    seriesdata.cpp

HEADERS += \
    timeseries.h \
    indexmanager.h \
    seriesmanager.h \
    seriesindex.h \
    seriesdata.h \
    types.h
