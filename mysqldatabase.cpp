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
    cout<<"Mysql is starded !!"<<endl;

}

InternalMessage *MysqlDatabase::getResult(InternalMessage *p)
{
    cout<<"Creating internal message!!"<<endl;
    if(p!=NULL){
        NetworkAPI t=p->getMsg()->getHdr().getApiId();
        switch (t) {
        case NetworkAPI::RequestStatus:
            return getRadnikStatus(p);
            break;
        case NetworkAPI::JobStart:
            cout<<"Message type ulaz with time "<<dateTime.toString().toStdString()<<endl;
            radnikUlaz(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case NetworkAPI::JobEnd:
            cout<<"Message type izlaz with time "<<dateTime.toString().toStdString()<<endl;
            radnikIzlaz(p->getRadnikId(),dateTime);
            return NULL;
            break;

        case NetworkAPI::WorkOutStart:
            cout<<"Message type teren pocetak with time "<<dateTime.toString().toStdString()<<endl;
            radnikTeren(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case NetworkAPI::WorkOutEnd:
            cout<<"Message type teren kraj with time "<<dateTime.toString().toStdString()<<endl;
            radnikTerenKraj(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case NetworkAPI::PauseStart:
            cout<<"Message type pauza pocetak with time "<<dateTime.toString().toStdString()<<endl;
            radnikPauza(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case NetworkAPI::PauseEnd:
            cout<<"Message type pauza kraj with time "<<dateTime.toString().toStdString()<<endl;
            radnikPauzaKraj(p->getRadnikId(),dateTime);
            return NULL;
            break;

        case NetworkAPI::PrivateStart:
            cout<<"Message type privatno pocetak with time "<<dateTime.toString().toStdString()<<endl;
            radnikPrivatno(p->getRadnikId(),dateTime);
            return NULL;
            break;
        case NetworkAPI::PrivateEnd:
            cout<<"Message type privatno kraj with time "<<dateTime.toString().toStdString()<<endl;
            radnikPrivatnoKraj(p->getRadnikId(),dateTime);
            return NULL;
            break;
        default:
            cout<<"Unknow message type"<<endl;
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
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        db.close();
        db.open();
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        if(query->exec()){
            db.close();
            return true;
        }else{
            cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
            cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
            db.close();
            return false;
        }
    }
    else{
        cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
        cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
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
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=0 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        db.close();
        return true;
    }
    else{
        cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
        cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
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
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=3 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        db.close();
        return true;
    }
    else{
        cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
        cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
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
        cout<<query->lastError().text().toStdString()<<endl;
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
        cout<<query->lastError().text().toStdString()<<endl;
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
    cout<<"End current time status radnikId: "<<radnikId<<" status: "<<status<<" time: "<<time.toString().toStdString()<<endl;
    switch(status){

    case 2://pauza
        db.open();
        query->prepare("UPDATE PauzaEv RIGHT JOIN  Podaci on PauzaEv.MaticniBroj=Podaci.MaticniBroj  set PauzaDO=? where  PauzaEv.PauzaOD !=0 and Podaci.rfid=? and PauzaEv.PauzaDO=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        query->exec();
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        db.close();
        break;
    case 3://teren
        db.open();
        query->prepare("UPDATE Vrijeme LEFT JOIN  Podaci on Vrijeme.MB=Podaci.MaticniBroj  set TerenUlaz=? where Podaci.rfid=? and Vrijeme.TerenUlaz=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        query->exec();
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        db.close();
        break;
    case 4:// privatno
        db.open();
        query->prepare("UPDATE PauzaEv RIGHT JOIN  Podaci on PauzaEv.MaticniBroj=Podaci.MaticniBroj  set PrivatnoDO=? where PauzaEv.PrivatnoOD !=0 and Podaci.rfid=? and PauzaEv.PrivatnoDO=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        query->exec();
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        query->exec();
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
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
    cout<<"Get radnik status radnikid: "<<radnikId<<endl;
    db.open();
    query->prepare("select Vrijeme from Podaci where rfid=?");
    query->bindValue(0,radnikId);
    query->exec();
    cout<<"Query "<<query->lastQuery().toStdString()<<endl;
    db.close();
    while(query->next()){
        return query->value(0).toInt();
    }
    return -1;
}

InternalMessage *MysqlDatabase::getRadnikStatus(InternalMessage *p)
{

    EvNetEmployeeId *id=p->getMsg()->getPayload();
    cout<<"Get radnik status radnikid: "<<id->getRfId()<<endl;
    memset(radnikStatusBuffer,0x00,sizeof(radnikStatusBuffer));
    InternalMessage *msg=new InternalMessage;
    EvNetMessage *netMsg=EvNetMessageBuilder::createMsgWithHdr(NetworkAPI::RequestStatus);
    EvNetEmployeeInfo *info=new EvNetEmployeeInfo;
    netMsg->setPayload(info);
    msg->setMsg(netMsg);
    msg->setCmdType(3);
    msg->setFd(p->getFd());
    db.open();
    query->prepare("select Vrijeme,Ime,Prezime,Funkcija from Podaci where rfid=?");
    query->bindValue(0,id->getRfId());
    query->exec();
    cout<<"Query "<<query->lastQuery().toStdString()<<endl;
    db.close();
    while(query->next()){
        int vrijemeStatus=query->value(0).toInt();
        info->setStatus(*((EvNetEmployeeStatus)EvNetMessageBuilder::createEmplStatus((EmployeeStatus)vrijemeStatus));
        cout<<"Time status is "<<vrijemeStatus<<endl;
        info->setName(query->value(1).toString().toStdString().c_str());
        cout<<"Name: "<<info->getName()<<endl;

        info->setLastName(query->value(2).toString().toStdString().c_str());
        cout<<"Lastname: "<<info->getLastName()<<endl;

        info->setFunction(query->value(3).toString().toStdString().c_str());
        cout<<"Function: "<<info->getFunction();

        msg->setMsg(netMsg);

        cout<<"Data successful set "<<endl;
        return msg;
    }
    cout<<"No data for radnik id: "<<id->getRfId()<<endl;
    info->setStatus(EvNetMessageBuilder::createEmployeeStatus(*((EvNetEmployeeStatus)(EmployeeStatus)500)));
    return msg;

}

int MysqlDatabase::fillBuff(void *dst, void *src, int size)
{
    memcpy(dst,src,size);
    return size;
}
