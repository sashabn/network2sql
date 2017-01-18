TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    server.cpp \
    tcpserver.cpp \
    abstractdelegate.cpp \
    database.cpp \
    mysqldatabase.cpp \
    gtime.cpp \
    gdate.cpp

HEADERS += \
    server.h \
    tcpserver.h \
    definicije.h \
    abstractdelegate.h \
    database.h \
    mysqldatabase.h \
    gtime.h \
    gdate.h

LIBS += -lpthread -lmysqlcppconn
