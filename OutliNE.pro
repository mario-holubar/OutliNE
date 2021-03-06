#-------------------------------------------------
#
# Project created by QtCreator 2018-03-10T18:01:38
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OutliNE
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    NE/experiment.cpp \
    NE/racingtask.cpp \
    NE/sane.cpp \
    NE/task.cpp \
    UI/instancemodel.cpp \
    UI/main.cpp \
    UI/mainview.cpp \
    UI/mainwindow.cpp \
    UI/netview.cpp \
    UI/paramdialog.cpp \
    NE/esp.cpp \
    NE/ne.cpp \
    NE/cosyne.cpp \
    UI/performanceview.cpp

HEADERS += \
    NE/experiment.h \
    NE/racingtask.h \
    NE/sane.h \
    NE/task.h \
    UI/instancemodel.h \
    UI/mainview.h \
    UI/mainwindow.h \
    UI/netview.h \
    UI/paramdialog.h \
    NE/esp.h \
    NE/ne.h \
    NE/cosyne.h \
    UI/performanceview.h

FORMS += \
    mainwindow.ui

DISTFILES += \


RESOURCES += \
    icons.qrc
