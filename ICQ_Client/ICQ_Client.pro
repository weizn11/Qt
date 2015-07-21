#-------------------------------------------------
#
# Project created by QtCreator 2015-05-18T12:38:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ICQ_Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    loginwindow.cpp \
    basic/thread.cpp \
    commun/commun.cpp \
    p2p/p2p.cpp \
    basic/encode.cpp \
    registerwindow.cpp \
    friendstreeview.cpp \
    findfriendwindow.cpp \
    oneselfinfowindow.cpp \
    chatwindow.cpp \
    chatform.cpp \
    filesendform.cpp \
    filerecvform.cpp \
    ctooltip.cpp \
    creategroup.cpp \
    findgroupwindow.cpp \
    groupslistview.cpp

HEADERS  += mainwindow.h \
    loginwindow.h \
    basic/thread.h \
    commun/commun.h \
    p2p/p2p.h \
    global.h \
    basic/encode.h \
    registerwindow.h \
    friendstreeview.h \
    findfriendwindow.h \
    oneselfinfowindow.h \
    chatwindow.h \
    chatform.h \
    filesendform.h \
    filerecvform.h \
    ctooltip.h \
    creategroup.h \
    findgroupwindow.h \
    groupslistview.h

FORMS    += mainwindow.ui \
    loginwindow.ui \
    registerwindow.ui \
    findfriendwindow.ui \
    oneselfinfowindow.ui \
    chatwindow.ui \
    chatform.ui \
    filesendform.ui \
    filerecvform.ui \
    creategroup.ui \
    findgroupwindow.ui

RESOURCES += \
    res.qrc

LIBS    +=  \
        libws2_32

RC_FILE += icon.rc

DISTFILES += \
    icon.rc
