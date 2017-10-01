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
    bool radnikUlaz(long long int radnikId,QDateTime time);
    bool radnikIzlaz(long long int radnikId,QDateTime time);
    bool radnikTeren(long long int radnikId,QDateTime time);
    bool radnikTerenKraj(long long int radnikId,QDateTime time);
    bool radnikPauza(long long int radnikId,QDateTime time);
    bool radnikPauzaKraj(long long int radnikId,QDateTime time);
    bool radnikPrivatno(long long int radnikId,QDateTime time);
    bool radnikPrivatnoKraj(long long int radnikId,QDateTime time);
    bool krajCurrentStatus(long long int radnikId,int status,QDateTime time);
    int getRadnikStatus(long long int radnikId);

    //functions
    InternalMessage *getRadnikStatus(InternalMessage *p);
    static int fillBuff(void *dst,void *src,int size);
//    int constructBuffer()



};

#endif // MYSQLDATABASE_H
