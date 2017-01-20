TEMPLATE = app
QT += core sql
QT -= gui
CONFIG += console c++11
CONFIG -= app_bundle


SOURCES += main.cpp \
    server.cpp \
    tcpserver.cpp \
    abstractdelegate.cpp \
    database.cpp \
    mysqldatabase.cpp \
    gtime.cpp \
    gdate.cpp \
    messageparser.cpp \
    internalmessage.cpp

HEADERS += \
    server.h \
    tcpserver.h \
    definicije.h \
    abstractdelegate.h \
    database.h \
    mysqldatabase.h \
    gtime.h \
    gdate.h \
    messageparser.h \
    internalmessage.h

LIBS += -lpthread -lmysqlcppconn
