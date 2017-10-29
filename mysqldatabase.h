#ifndef MYSQLDATABASE_H
#define MYSQLDATABASE_H
#include "database.h"

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <QDateTime>
#include <internalmessage.h>
#include <messageparser.h>
#include <iostream>
#include <QDebug>
#include <QSqlError>
#include <evnetmessagebuilder.h>
#include <evnetmessage.h>



using namespace std;

class MysqlDatabase : public Database
{
public:
    MysqlDatabase(const QString &host,const QString &user,const QString &password,const QString &dbname);
    InternalMessage *getResult(InternalMessage *p);
private:
    QSqlDatabase db;
    QSqlQuery *query;

    QString dbHost;
    QString dbUser;
    QString dbPassword;
    QString dbDatabaseName;
    char radnikStatusBuffer[512];
    InternalMessage *connectionRequest(InternalMessage *p);
    InternalMessage *radnikUlaz(InternalMessage *p);
    InternalMessage *radnikIzlaz(InternalMessage *p);
    InternalMessage *radnikTeren(InternalMessage *p);
    InternalMessage *radnikTerenKraj(InternalMessage *p);
    InternalMessage *radnikPauza(InternalMessage *p);
    InternalMessage *radnikPauzaKraj(InternalMessage *p);
    InternalMessage *radnikPrivatno(InternalMessage *p);
    InternalMessage *radnikPrivatnoKraj(InternalMessage *p);
    EvNetGenericResponse *krajCurrentStatus(long long radnikId, int status, QDateTime time);
    int getRadnikStatus(long long radnikID);

    //functions
    InternalMessage *getRadnikStatus(InternalMessage *p);
    static int fillBuff(void *dst,void *src,int size);
//    int constructBuffer()

    InternalMessage *createGenericResp(InternalMessage* p);



};

#endif // MYSQLDATABASE_H
