QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/loginwindow.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/network/client.cpp \
    src/qasioeventdispatcher.cpp

HEADERS += \
    src/include/loginwindow.h \
    src/include/mainwindow.h \
    src/include/chat_message.hpp \
    src/include/qasioeventdispatcher.hpp \
    src/include/utils.hpp

FORMS += \
    src/ui/loginwindow.ui \
    src/ui/mainwindow.ui

TRANSLATIONS += \
    translations/TMsgNow_zh_HK.ts
CONFIG += lrelease
CONFIG += embed_translations

INCLUDEPATH += "/opt/homebrew/Cellar/boost/1.78.0_1/include"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
