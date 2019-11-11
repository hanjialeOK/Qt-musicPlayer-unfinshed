#-------------------------------------------------
#
# Project created by QtCreator 2019-02-05T17:55:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hello_world
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

CONFIG += c++11

SOURCES += \
        main.cpp \
    main_window.cpp \
    widget_Bar/list_bar.cpp \
    widget_Bar/musicinfo_bar.cpp \
    widget_Bar/player_bar.cpp \
    widget_Bar/title_bar.cpp \
    widget_Helper/draganddrop.cpp \
    widget_Helper/framelesswindow.cpp \
    widget_Children/widget.cpp \
    widget_Children/widget_1.cpp \
    widget_Children/widget_localmusic.cpp \
    widget_Helper/arrowwidget.cpp \
    test.cpp \
    widget_Helper/mpeghelper.cpp \
    widget_Helper/myclass.cpp

HEADERS += \
    main_window.h \
    widget_Bar/list_bar.h \
    widget_Bar/musicinfo_bar.h \
    widget_Bar/player_bar.h \
    widget_Bar/title_bar.h \
    widget_Helper/draganddrop.h \
    widget_Helper/framelesswindow.h \
    widget_Children/widget.h \
    widget_Children/widget_1.h \
    widget_Children/widget_localmusic.h \
    widget_Helper/arrowwidget.h \
    test.h \
    widget_Helper/mpeghelper.h \
    widget_Helper/myclass.h

FORMS += \
        widget.ui \
    widget_1.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

QT += multimedia
QT += winextras
LIBS += -ldwmapi

win32: LIBS += -L$$PWD/_taglibs/ -llibtag.dll

INCLUDEPATH += $$PWD/_taglibs/include
DEPENDPATH += $$PWD/_taglibs/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/_taglibs/libtag.dll.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/_taglibs/libtag.dll.a
