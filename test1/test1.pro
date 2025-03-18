#-------------------------------------------------
#
# Project created by QtCreator 2024-09-02T14:31:31
#
#-------------------------------------------------

QT       += core gui serialport sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test1
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

INCLUDEPATH += D:\Opencv\build_qt\install\include
INCLUDEPATH += D:\Opencv\build_qt\install\include\opencv
INCLUDEPATH += D:\Opencv\build_qt\install\include\opencv2
LIBS += -L D:\Opencv\build_qt\install\x86\mingw\lib\libopencv_*.a


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    home.cpp \
    sqldatabase.cpp

HEADERS += \
        mainwindow.h \
    home.h \
    sqldatabase.h

FORMS += \
        mainwindow.ui \
    home.ui

RESOURCES += \
    image.qrc
