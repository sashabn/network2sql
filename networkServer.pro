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
    internalmessage.cpp \
    messageasyncproccessor.cpp \
    fileprovider.cpp \
    messagequeue.cpp \
    evprotocolheader.cpp \
    evprotocolmsg.cpp \
    evprotocolpayload.cpp \
    networkdecoder.cpp

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
    internalmessage.h \
    messageasyncproccessor.h \
    fileprovider.h \
    messagequeue.h \
    evprotocolheader.h \
    evprotocolmsg.h \
    evprotocolpayload.h \
    networkdecoder.h

LIBS += -lpthread -lmysqlcppconn
QMAKE_RPATHDIR += ./

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../disk2/projekti/ucenje/libsEv/libs/release/ -levidencijaNetworkProtocolLibrary
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../disk2/projekti/ucenje/libsEv/libs/debug/ -levidencijaNetworkProtocolLibrary
else:unix: LIBS += -L$$PWD/../../disk2/projekti/ucenje/libsEv/libs/ -levidencijaNetworkProtocolLibrary

INCLUDEPATH += $$PWD/../../disk2/projekti/ucenje/libsEv/include
DEPENDPATH += $$PWD/../../disk2/projekti/ucenje/libsEv/include
