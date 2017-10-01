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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -levidencijaNetworkProtocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -levidencijaNetworkProtocol
else:unix: LIBS += -L$$PWD/./ -levidencijaNetworkProtocol
else:unix: LIBS += -L$$PWD/./ -levidencijaNetworkProtocol.so.1

INCLUDEPATH += $$PWD/../Documents/projekti/evPro/include
DEPENDPATH += $$PWD/../Documents/projekti/evPro/include
