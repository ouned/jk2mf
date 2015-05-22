#-------------------------------------------------
#
# Project created by QtCreator 2012-08-23T12:38:34
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += static

QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++

TARGET = WinClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

win32:RC_FILE = res.rc

OTHER_FILES += \
    res.rc
