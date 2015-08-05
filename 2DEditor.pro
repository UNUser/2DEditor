#-------------------------------------------------
#
# Project created by QtCreator 2015-07-21T12:25:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 2DEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vertex.cpp \
    edge.cpp \
    scene.cpp \
    animationcontroller.cpp \
    frame.cpp

HEADERS  += mainwindow.h \
    vertex.h \
    edge.h \
    scene.h \
    animationcontroller.h \
    frame.h

FORMS    += mainwindow.ui

RESOURCES += \
    Images.qrc

OTHER_FILES +=
