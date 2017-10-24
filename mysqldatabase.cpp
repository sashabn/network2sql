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
        case NetworkAPI::RequestEmployeeInfo:
            return getRadnikStatus(p);
            break;
        case NetworkAPI::JobStart:
            cout<<"Message type ulaz with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikUlaz(p);
            break;
        case NetworkAPI::JobEnd:
            cout<<"Message type izlaz with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikIzlaz(p);
            break;

        case NetworkAPI::WorkOutStart:
            cout<<"Message type teren pocetak with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikTeren(p);
            break;
        case NetworkAPI::WorkOutEnd:
            cout<<"Message type teren kraj with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikTerenKraj(p);
            break;
        case NetworkAPI::PauseStart:
            cout<<"Message type pauza pocetak with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikPauza(p);
            break;
        case NetworkAPI::PauseEnd:
            cout<<"Message type pauza kraj with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikPauzaKraj(p);
            break;

        case NetworkAPI::PrivateStart:
            cout<<"Message type privatno pocetak with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikPrivatno(p);
            break;
        case NetworkAPI::PrivateEnd:
            cout<<"Message type privatno kraj with time "<<((EvNetTimeInfo*)p->getMsg()->getPayload())->getTime()<<endl;
            return radnikPrivatnoKraj(p);
            break;
        case NetworkAPI::NetworkConnectionRequest:
            return connectionRequest(p);
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

InternalMessage *MysqlDatabase::connectionRequest(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    return msg;
}

InternalMessage * MysqlDatabase::radnikUlaz(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    int currentStatus=getRadnikStatus(timeInfo->getRfid()->getRfId());
    if(currentStatus==(int)OnJob){
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::AlreadyExist);
        response->setDetail("Update time error: Employee is already on job");
    }
    db.open();
    query->prepare("insert into Vrijeme (MaticniBroj,Datum,Ulaz,Izlaz,MB) values ((select MaticniBroj from Podaci where rfid=?),?,?,0,(select MaticniBroj from Podaci where rfid=?))");
    query->bindValue(0,timeInfo->getRfid()->getRfId());
    query->bindValue(1,dateTime.date());
    query->bindValue(2,dateTime);
    query->bindValue(3,timeInfo->getRfid()->getRfId());
    if(query->exec()){
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        db.close();
        db.open();
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,timeInfo->getRfid()->getRfId());
        if(query->exec()){
            db.close();
            response->setStatus(GenericResponse::Success);
            response->setCause(GenericResponseCause::NoError);
            response->setDetail(query->lastQuery().toStdString().c_str());
            return msg;
        }else{
            cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
            cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            db.close();
            return msg;
        }
    }
    else{
        cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
        cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::SqlError);
        response->setDetail(query->lastError().text().toStdString().c_str());
        db.close();
        return msg;
    }
}

InternalMessage *MysqlDatabase::radnikIzlaz(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    char *radnikId=timeInfo->getRfid()->getRfId();
    int currentStatus=getRadnikStatus(radnikId);
    EvNetGenericResponse *status=NULL;
    if((status=krajCurrentStatus((const char*)radnikId,currentStatus,dateTime))->getStatus()==GenericResponse::Fail){
        msg->getMsg()->setPayload(status);
        return msg;
    }
    msg->getMsg()->setPayload(status);
    db.open();
    query->prepare("UPDATE Vrijeme LEFT JOIN  Podaci on Vrijeme.MB=Podaci.MaticniBroj  set Izlaz=? where Podaci.rfid=? and Vrijeme.Izlaz=0");
    query->bindValue(0,dateTime);
    query->bindValue(1,radnikId);
    if(query->exec()){
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=0 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return msg;
        }
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        db.close();
        return msg;
    }else{
        cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
        cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::SqlError);
        response->setDetail(query->lastError().text().toStdString().c_str());
        db.close();
        return msg;
    }
}

InternalMessage *MysqlDatabase::radnikTeren(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    char *radnikId=timeInfo->getRfid()->getRfId();
    int currentStatus=getRadnikStatus(radnikId);
    if(currentStatus==(int)WorkOut){
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::AlreadyExist);
        response->setDetail("Update time error: Employee is already on workout");
        return msg;
    }
    if(currentStatus==0){
        InternalMessage *respUlaz=radnikUlaz(p);
        if(((EvNetGenericResponse*)respUlaz->getMsg()->getPayload())->getStatus()!=GenericResponse::Success){
            delete msg;
            return respUlaz;
        }else{
            delete respUlaz;
        }
    }else{
        EvNetGenericResponse *status=krajCurrentStatus((const char*)radnikId,currentStatus,dateTime);
        if(status->getStatus()==GenericResponse::Fail){
            msg->getMsg()->setPayload(status);
            return msg;
        }else{
            msg->getMsg()->setPayload(status);
        }
    }
    db.open();
    query->prepare("insert into Vrijeme (MaticniBroj,Datum,TerenIzlaz,TerenUlaz,MB) values ((select MaticniBroj from Podaci where rfid=?),?,?,0,(select MaticniBroj from Podaci where rfid=?))");
    query->bindValue(0,radnikId);
    query->bindValue(1,dateTime.date());
    query->bindValue(2,dateTime);
    query->bindValue(3,radnikId);
    if(query->exec()){
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=3 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return msg;
        }
        cout<<"Query success "<<query->lastQuery().toStdString()<<endl;
        db.close();
        return msg;
    }
    else{
        cout<<"Query unsuccess "<<query->lastQuery().toStdString()<<endl;
        cout<<"Query error "<<query->lastError().text().toStdString()<<endl;
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::SqlError);
        response->setDetail(query->lastError().text().toStdString().c_str());
        db.close();
        return msg;
    }
}

InternalMessage *MysqlDatabase::radnikTerenKraj(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    char *radnikId=timeInfo->getRfid()->getRfId();
    EvNetGenericResponse *resp= krajCurrentStatus(radnikId,3,dateTime);
    msg->getMsg()->setPayload(resp);
    return msg;
}

InternalMessage *MysqlDatabase::radnikPauza(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    char *radnikId=timeInfo->getRfid()->getRfId();
    int currentStatus=getRadnikStatus(radnikId);
    if(currentStatus==(int)OnPause){
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::AlreadyExist);
        response->setDetail("Update time error: Employee is already on pause");
        return msg;
    }
    EvNetGenericResponse *status=NULL;
    if((status=krajCurrentStatus((const char*)radnikId,currentStatus,dateTime))->getStatus()==GenericResponse::Fail){
        msg->getMsg()->setPayload(status);
        return msg;
    }
    msg->getMsg()->setPayload(status);
    db.open();
    query->prepare("insert into PauzaEv (MaticniBroj,MB,Datum,PauzaOD,PauzaDO) values ((select MaticniBroj from Podaci where rfid=?),(select MaticniBroj from Podaci where rfid=?),?,?,0)");
    query->bindValue(0,radnikId);
    query->bindValue(1,radnikId);
    query->bindValue(2,dateTime.date());
    query->bindValue(3,dateTime);

    if(query->exec()){
        query->prepare("update Podaci set Vrijeme=2 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return msg;
        }
        db.close();
        return msg;
    }
    else{
        cout<<query->lastError().text().toStdString()<<endl;
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::SqlError);
        response->setDetail(query->lastError().text().toStdString().c_str());
        db.close();
        return msg;
    }
}

InternalMessage *MysqlDatabase::radnikPauzaKraj(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    char *radnikId=timeInfo->getRfid()->getRfId();
    EvNetGenericResponse *resp= krajCurrentStatus(radnikId,2,dateTime);
    msg->getMsg()->setPayload(resp);
    return msg;
}

InternalMessage * MysqlDatabase::radnikPrivatno(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    char *radnikId=timeInfo->getRfid()->getRfId();
    int currentStatus=getRadnikStatus(radnikId);
    if(currentStatus==(int)OnPrivate){
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::AlreadyExist);
        response->setDetail("Update time error: Employee is already on private");
        return msg;
    }
    EvNetGenericResponse *status=NULL;
    if((status=krajCurrentStatus((const char*)radnikId,currentStatus,dateTime))->getStatus()==GenericResponse::Fail){
        msg->getMsg()->setPayload(status);
        return msg;
    }
    msg->getMsg()->setPayload(status);
    db.open();
    query->prepare("insert into PauzaEv (MaticniBroj,MB,Datum,PrivatnoOD,PrivatnoDO) values ((select MaticniBroj from Podaci where rfid=?),(select MaticniBroj from Podaci where rfid=?),?,?,0)");
    query->bindValue(0,radnikId);
    query->bindValue(1,radnikId);
    query->bindValue(2,dateTime.date());
    query->bindValue(3,dateTime);

    if(query->exec()){
        query->prepare("update Podaci set Vrijeme=4 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return msg;
        }
        db.close();
        return msg;
    }
    else{
        cout<<query->lastError().text().toStdString()<<endl;
        response->setStatus(GenericResponse::Fail);
        response->setCause(GenericResponseCause::SqlError);
        response->setDetail(query->lastError().text().toStdString().c_str());
        db.close();
        return msg;
    }
}

InternalMessage *MysqlDatabase::radnikPrivatnoKraj(InternalMessage *p)
{
    InternalMessage *msg=createGenericResp(p);
    EvNetGenericResponse *response = (EvNetGenericResponse*)msg->getMsg()->getPayload();
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)p->getMsg()->getPayload();
    msg->getMsg()->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    QDateTime dateTime=QDateTime::fromString(QString::fromStdString(timeInfo->getTime()),DATE_TIME_FORMAT);
    char *radnikId=timeInfo->getRfid()->getRfId();
    EvNetGenericResponse *resp= krajCurrentStatus(radnikId,4,dateTime);
    msg->getMsg()->setPayload(resp);
    return msg;
}

EvNetGenericResponse *MysqlDatabase::krajCurrentStatus(const char* radnikId, int status, QDateTime time)
{
    EvNetGenericResponse *response=new EvNetGenericResponse;
    response->setCause(GenericResponseCause::NoError);
    response->setStatus(GenericResponse::Success);
    cout<<"End current time status radnikId: "<<radnikId<<" status: "<<status<<" time: "<<time.toString().toStdString()<<endl;
    switch(status){

    case 2://pauza
        db.open();
        query->prepare("UPDATE PauzaEv RIGHT JOIN  Podaci on PauzaEv.MaticniBroj=Podaci.MaticniBroj  set PauzaDO=? where  PauzaEv.PauzaOD !=0 and Podaci.rfid=? and PauzaEv.PauzaDO=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            return response;
        }
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return response;
        }
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        db.close();
        break;
    case 3://teren
        db.open();
        query->prepare("UPDATE Vrijeme LEFT JOIN  Podaci on Vrijeme.MB=Podaci.MaticniBroj  set TerenUlaz=? where Podaci.rfid=? and Vrijeme.TerenUlaz=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return response;
        }
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return response;
        }
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        db.close();
        break;
    case 4:// privatno
        db.open();
        query->prepare("UPDATE PauzaEv RIGHT JOIN  Podaci on PauzaEv.MaticniBroj=Podaci.MaticniBroj  set PrivatnoDO=? where PauzaEv.PrivatnoOD !=0 and Podaci.rfid=? and PauzaEv.PrivatnoDO=0");
        query->bindValue(0,time);
        query->bindValue(1,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return response;
        }
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        query->prepare("update Podaci set Vrijeme=1 where rfid=?");
        query->bindValue(0,radnikId);
        if(!query->exec()){
            response->setStatus(GenericResponse::Fail);
            response->setCause(GenericResponseCause::SqlError);
            response->setDetail(query->lastError().text().toStdString().c_str());
            cout<<"Query "<<query->lastQuery().toStdString()<<endl;
            return response;
        }
        cout<<"Query "<<query->lastQuery().toStdString()<<endl;
        db.close();
        break;
    default:
        return response;
        break;
    }

    return response;
}

int MysqlDatabase::getRadnikStatus(const char *radnikID)
{
    cout<<"Get radnik status radnikid: "<<radnikID<<endl;
    db.open();
    query->prepare("select Vrijeme from Podaci where rfid=?");
    query->bindValue(0,radnikID);
    if(!query->exec()){
        return -1;
    }
    cout<<"Query "<<query->lastQuery().toStdString()<<endl;
    db.close();
    while(query->next()){
        return query->value(0).toInt();
    }
    return -1;
}

InternalMessage *MysqlDatabase::getRadnikStatus(InternalMessage *p)
{

    EvNetEmployeeId *id=(EvNetEmployeeId *)p->getMsg()->getPayload();
    cout<<"Get radnik status radnikid: "<<id->getRfId()<<endl;
    memset(radnikStatusBuffer,0x00,sizeof(radnikStatusBuffer));
    InternalMessage *msg=new InternalMessage;
    EvNetEmployeeInfo *info=new EvNetEmployeeInfo;
    info->setRfid(id);
    EvNetMessage *netMsg=EvNetMessageBuilder::createMsgWithHdr(NetworkAPI::RequestStatus);
    netMsg->getHdr().setTransId(p->getMsg()->getHdr().getTransId());
    netMsg->setPayload(info);
    msg->setMsg(netMsg);
    msg->setCmdType(3);
    msg->setFd(p->getFd());
    info->setStatus((EvNetEmployeeStatus*)EvNetMessageBuilder::createEmplStatus(EmployeeStatus::NotFoundInDatabase));
    if(strlen(id->getRfId())<2){
        return msg;
    }
    db.open();
    query->prepare("select Vrijeme,Ime,Prezime,Funkcija,MaticniBroj from Podaci where rfid=?");
    query->bindValue(0,id->getRfId());
    query->exec();
    cout<<"Query "<<query->lastQuery().toStdString()<<endl;
    db.close();
    while(query->next()){
        int vrijemeStatus=query->value(0).toInt();
        info->setStatus((EvNetEmployeeStatus*)EvNetMessageBuilder::createEmplStatus((EmployeeStatus)vrijemeStatus));
        cout<<"Time status is "<<vrijemeStatus<<endl;
        info->setName(query->value(1).toString().toStdString().c_str());
        cout<<"Name: "<<info->getName()<<endl;

        info->setLastName(query->value(2).toString().toStdString().c_str());
        cout<<"Lastname: "<<info->getLastName()<<endl;

        info->setFunction(query->value(3).toString().toStdString().c_str());
        cout<<"Function: "<<info->getFunction();

        info->setId(query->value(4).toString().toStdString().c_str());

        msg->setMsg(netMsg);

        cout<<"Data successful set "<<endl;
        return msg;
    }
    cout<<"No data for radnik id: "<<id->getRfId()<<endl;

    return msg;

}

int MysqlDatabase::fillBuff(void *dst, void *src, int size)
{
    memcpy(dst,src,size);
    return size;
}

InternalMessage *MysqlDatabase::createGenericResp(InternalMessage *p)
{
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(p->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(p->getMsg()->getHdr().getApiId()));
    EvNetGenericResponse *response=new EvNetGenericResponse;
    response->setCause(GenericResponseCause::NoError);
    response->setStatus(GenericResponse::Success);
    msg->getMsg()->setPayload(response);
    return msg;
}
