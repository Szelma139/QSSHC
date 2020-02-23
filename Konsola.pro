QT += gui core network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SUBDIRS = \
    libs

SOURCES += main.cpp \
    mainwindow.cpp \
    qsshsocket.cpp \
    form.cpp \
    tabdialog.cpp \
    keymap.cpp \
    mywidget.cpp \
    mainsettings.cpp


FORMS += \
    mainwindow.ui \
    form.ui \
    mywidget.ui

HEADERS += \
    mainwindow.h \
    qsshsocket.h \
    form.h \
    tabdialog.h \
    keymap.h \
    mywidget.h \
    mainsettings.h



RESOURCES += \
    Configuration/conf.qrc \
    Configuration/stylesheet.qrc






win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/release/ -lssh
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/debug/ -lssh
else:unix: LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/ -lssh

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

