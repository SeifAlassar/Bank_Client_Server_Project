
TARGET = Tcp_Server
CONFIG += c++11
QT -= gui
QT += network




TEMPLATE = app

SOURCES += \
    JsonDatabase.cpp \
    main.cpp \
    tcpserver.cpp

HEADERS += \
    JsonDatabase.h \
    tcpserver.h
