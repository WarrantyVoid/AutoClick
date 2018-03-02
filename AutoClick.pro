#-------------------------------------------------
#
# Project created by QtCreator 2016-12-26T18:03:07
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoClick
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp \
    MainWindow.cpp \
    MouseRobot.cpp \
    MouseRule.cpp \
    SizedLineEdit.cpp \
    GlassWindow.cpp \
    LimitedKeySequence.cpp \
    MouseRuleConfig.cpp

HEADERS  += \
    MainWindow.hpp \
    MouseRobot.hpp \
    MouseRule.hpp \
    SizedLineEdit.hpp \
    GlassWindow.hpp \
    LimitedKeySequence.hpp \
    MouseRuleConfig.hpp

FORMS    += \
    MainWindow.ui \
    MouseRule.ui \
    MainWidget.ui \
    MainDialog.ui

RESOURCES += \
    icons.qrc

LIBS += -lX11 -lXtst

LIBS += -L$$PWD/../build-uglobalhotkey-Desktop_Qt_5_7_0_GCC_64bit-Debug/ -lUGlobalHotkey
INCLUDEPATH += $$PWD/../GlobalHotkey
DEPENDPATH += $$PWD/../GlobalHotkey
