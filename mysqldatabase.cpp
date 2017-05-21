#include "mysqldatabase.h"



MysqlDatabase::MysqlDatabase(const QString &host, const QString &user, const QString &password, const QString &dbname):dbHost(host),
    dbUser(user),dbPassword(password),dbDatabaseName(dbname)
{
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbDatabaseName);
    db.setUserName(dbUser);
    db.setPassword(dbPassword);
    db.setHostName(dbHost);
    query=new QSqlQuery(db);

}

InternalMessage *MysqlDatabase::getResult(MessageParser *p)
{
    cout<<"AAA"<<endl;
    if(p!=NULL){
        qint64 epochTime=0;
        QDateTime dateTime;
        if(p->getTime()!=NULL){
            qDebug()<<"A OVO JE VREME STRING "<<p->getTime();
            epochTime=QString::fromLatin1(p->getTime()).toLongLong();
            qDebug()<<"EPOCH TIME "<<epochTime;
            dateTime=QDateTime::fromMSecsSinceEpoch(epochTime);
        }
        MessageType t=p->getMessageType();
        switch (t) {
        case MessageType::RequestStatus:
            return getRadnikStatus(p);
            break;
        case MessageType::Ulaz:
            qDebug()<<dateTime.toString()<<"OVO JE QDATE VREME";
            radnikUlaz(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case MessageType::Izlaz:
            radnikIzlaz(p->getRadnikId(),dateTime);
            return NULL;
            break;

        case MessageType::TerenPocetak:
            radnikTeren(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case MessageType::TerenKraj:
            radnikTerenKraj(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case MessageType::PauzaPocetak:
            radnikPauza(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case MessageType::PauzaKraj:
            radnikPauzaKraj(p->getRadnikId(),dateTime);
            return NULL;
            break;

        case MessageType::PrivatnoPocetak:
            radnikPrivatno(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case MessageType::PrivatnoKraj:
            radnikPrivatnoKraj(p->getRadnikId(),dateTime);
            return NULL;
            break;
        default:
            return NULL;
            break;
        }
    }else{
        cout<<"MESSAGE PARSER JE NULL"<<endl;
        return NULL;
    }
}

bool MysqlDatabase::radnikUlaz(long long int radnikId, QDateTime time)
{
    db.open();
    query->prepare("insert into Vrijeme (MaticniBroj,Datum,Ulaz,Izlaz,MB) values ((select MaticniBroj from Podaci where rfid=?),?,?,0,(select MaticniBroj from Podaci where rfid=?))");
    query->bindValue(0,radnikId);
    query->bindValue(1,time.date());
    query->bindValue(2,time);
    query->bindValue(3,radnikId);
    if(query->exec()){
        db.close();
        db.open();
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        if(query->exec()){
            db.close();
            return true;
        }else{
            db.close();
            return false;
        }
    }
    else{
        qDebug()<<query->lastError().text();
        db.close();
        return false;
    }
}

bool MysqlDatabase::radnikIzlaz(long long int radnikId, QDateTime time)
{
    int currentStatus=getRadnikStatus(radnikId);
    krajCurrentStatus(radnikId,currentStatus,time);
    db.open();
    query->prepare("UPDATE Vrijeme LEFT JOIN  Podaci on Vrijeme.MB=Podaci.MaticniBroj  set Izlaz=? where Podaci.rfid=? and Vrijeme.Izlaz=0");
    query->bindValue(0,time);
    query->bindValue(1,radnikId);
    if(query->exec()){
        query->prepare("update Podaci set Vrijeme=0 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        return true;
    }
    else{
        qDebug()<<query->lastError().text();
        db.close();
        return false;
    }
}

bool MysqlDatabase::radnikTeren(long long int radnikId, QDateTime time)
{
    int currentStatus=getRadnikStatus(radnikId);
    if(currentStatus==0){
        radnikUlaz(radnikId,time);
    }else{
        krajCurrentStatus(radnikId,currentStatus,time);
    }
    db.open();
    query->prepare("insert into Vrijeme (MaticniBroj,Datum,TerenIzlaz,TerenUlaz,MB) values ((select MaticniBroj from Podaci where rfid=?),?,?,0,(select MaticniBroj from Podaci where rfid=?))");
    query->bindValue(0,radnikId);
    query->bindValue(1,time.date());
    query->bindValue(2,time);
    query->bindValue(3,radnikId);
    if(query->exec()){
        query->prepare("update Podaci set Vrijeme=3 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        return true;
    }
    else{
        db.close();
        return false;
    }
}

bool MysqlDatabase::radnikTerenKraj(long long int radnikId, QDateTime time)
{
    krajCurrentStatus(radnikId,3,time);
}

bool MysqlDatabase::radnikPauza(long long int radnikId, QDateTime time)
{
    int currentStatus=getRadnikStatus(radnikId);
    krajCurrentStatus(radnikId,currentStatus,time);
    db.open();
    query->prepare("insert into PauzaEv (MaticniBroj,MB,Datum,PauzaOD,PauzaDO) values ((select MaticniBroj from Podaci where rfid=?),(select MaticniBroj from Podaci where rfid=?),?,?,0)");
    query->bindValue(0,radnikId);
    query->bindValue(1,radnikId);
    query->bindValue(2,time.date());
    query->bindValue(3,time);

    if(query->exec()){
        query->prepare("update Podaci set Vrijeme=2 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        return true;
    }
    else{
        db.close();
        return false;
    }
}

bool MysqlDatabase::radnikPauzaKraj(long long int radnikId, QDateTime time)
{
    krajCurrentStatus(radnikId,2,time);
}

bool MysqlDatabase::radnikPrivatno(long long int radnikId, QDateTime time)
{
    int currentStatus=getRadnikStatus(radnikId);
    krajCurrentStatus(radnikId,currentStatus,time);
    db.open();
    query->prepare("insert into PauzaEv (MaticniBroj,MB,Datum,PrivatnoOD,PrivatnoDO) values ((select MaticniBroj from Podaci where rfid=?),(select MaticniBroj from Podaci where rfid=?),?,?,0)");
    query->bindValue(0,radnikId);
    query->bindValue(1,radnikId);
    query->bindValue(2,time.date());
    query->bindValue(3,time);

    if(query->exec()){
        query->prepare("update Podaci set Vrijeme=4 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        return true;
    }
    else{
        db.close();
        return false;
    }
}

bool MysqlDatabase::radnikPrivatnoKraj(long long int radnikId, QDateTime time)
{
    krajCurrentStatus(radnikId,4,time);
}

bool MysqlDatabase::krajCurrentStatus(long long int radnikId, int status, QDateTime time)
{
    switch(status){

    case 2://pauza
        db.open();
        query->prepare("UPDATE PauzaEv RIGHT JOIN  Podaci on PauzaEv.MaticniBroj=Podaci.MaticniBroj  set PauzaDO=? where  PauzaEv.PauzaOD !=0 and Podaci.rfid=? and PauzaEv.PauzaDO=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        query->exec();
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        break;
    case 3://teren
        db.open();
        query->prepare("UPDATE Vrijeme LEFT JOIN  Podaci on Vrijeme.MB=Podaci.MaticniBroj  set TerenUlaz=? where Podaci.rfid=? and Vrijeme.TerenUlaz=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        query->exec();
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        break;
    case 4:// privatno
        db.open();
        query->prepare("UPDATE PauzaEv RIGHT JOIN  Podaci on PauzaEv.MaticniBroj=Podaci.MaticniBroj  set PrivatnoDO=? where PauzaEv.PrivatnoOD !=0 and Podaci.rfid=? and PauzaEv.PrivatnoDO=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        query->exec();
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        db.close();
        break;
    default:
        return true;
        break;
    }

    return true;
}

int MysqlDatabase::getRadnikStatus(long long int radnikId)
{
    db.open();
    query->prepare("select Vrijeme from Podaci where rfid=?");
    query->bindValue(0,radnikId);
    query->exec();
    db.close();
    while(query->next()){
        return query->value(0).toInt();
    }
    return -1;
}

InternalMessage *MysqlDatabase::getRadnikStatus(MessageParser *p)
{
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setSenderFd(p->getSenderFd());
    msg->setRfif(p->getRadnikId());
    db.open();
    query->prepare("select Vrijeme,Ime,Prezime,Funkcija from Podaci where rfid=?");
    query->bindValue(0,p->getRadnikId());
    query->exec();
    db.close();
    while(query->next()){
        int vrijemeStatus=query->value(0).toInt();
        cout<<"VRIJEME STATUTS JE "<<vrijemeStatus<<endl;
        char *ime=new char[query->value(1).toString().toStdString().size()+1];
        strcpy(ime,query->value(1).toString().toStdString().c_str());

        char *prezime=new char[query->value(2).toString().toStdString().size()+1];
        strcpy(prezime,query->value(2).toString().toStdString().c_str());

        char *funkcija=new char[query->value(3).toString().toStdString().size()+1];
        strcpy(funkcija,query->value(3).toString().toStdString().c_str());
        cout<<"IME "<<ime<<endl;

        vrijemeStatus=htonl(vrijemeStatus);
        char buf[sizeof(int)/*id odg*/+sizeof(long long int)/*id radnika*/+sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke*/+sizeof(int)/*poruka status*/
                +sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke ime*/+strlen(ime)/*poruka ime*/
                +sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke prezime*/+strlen(prezime)/*poruka prezime*/
                +sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke funkcija*/+strlen(funkcija)/*poruka funkcija*/];
        char *ptrbuf=buf;
        int iTmp=p->getMessageType();
        iTmp=htonl(iTmp);
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));

        long long int lTmp=p->getRadnikId();
        lTmp=htobe64(lTmp);
        ptrbuf+=fillBuff(ptrbuf,&lTmp,sizeof(lTmp));

        iTmp=1;
        iTmp=htonl(iTmp);
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));

        iTmp=sizeof(vrijemeStatus);
        iTmp=htonl(iTmp);
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));

        ptrbuf+=fillBuff(ptrbuf,&vrijemeStatus,sizeof(vrijemeStatus));
        //ime
        iTmp=htonl(3);
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));

        iTmp=htonl(strlen(ime));
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));
        ptrbuf+=fillBuff(ptrbuf,(void*)ime,strlen(ime));
        cout<<ime<<endl;
        //prezime
        iTmp=htonl(3);
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));

        iTmp=htonl(strlen(prezime));
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));
        ptrbuf+=fillBuff(ptrbuf,(void*)prezime,strlen(prezime));

        //funkcija
        iTmp=htonl(3);
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));

        iTmp=htonl(strlen(funkcija));
        ptrbuf+=fillBuff(ptrbuf,&iTmp,sizeof(iTmp));
        ptrbuf+=fillBuff(ptrbuf,(void*)funkcija,strlen(funkcija));




        msg->setData(buf,sizeof(buf));
        delete ime;
        return msg;
    }
    msg->setData("FALSE",strlen("FALSE"));
    return msg;

}

int MysqlDatabase::fillBuff(void *dst, void *src, int size)
{
    memcpy(dst,src,size);
    return size;
}
