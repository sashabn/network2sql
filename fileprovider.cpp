#include "fileprovider.h"

FileProvider::FileProvider()
{
    picLoc="/opt/pictures/";
}

InternalMessage *FileProvider::getRadnikPicture(InternalMessage *r)
{
    EvNetEmployeeId *id=(EvNetEmployeeId*)r->getMsg()->getPayload();
    cout<<"Get profile picture for radnikId: "<<id->getRfid()<<endl;
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(r->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(r->getMsg()->getHdr().getApiId()));
    msg->getMsg()->getHdr().setTransId(r->getMsg()->getHdr().getTransId());

    EvNetEmployeePicture *empPic=new EvNetEmployeePicture;


    string tmp=picLoc;
    empPic->setRfid(new EvNetEmployeeId(*id));
    empPic->setPicType(PictureTypeContainer::JPEG);
    tmp.append(QString::number(id->getRfid()).toStdString().c_str());
    tmp.append(".jpg");
    empPic->setPictureName(tmp.c_str());
    cout<<"Picture location "<<tmp<<endl;

    ifstream picFile(tmp);
    if(picFile.is_open()){
        int picSize;
        int beg=picFile.tellg();
        picFile.seekg(0,ios_base::end);
        picSize=picFile.tellg();
        picSize=picSize-beg;
        picFile.seekg(ios_base::beg);
        cout<<"Picture bytes count "<<picSize<<endl;
        char *picBuff=new char[picSize];
        if(picBuff==NULL){
            return msg;
        }
        empPic->setPictureSize(picSize);
        picFile.read(picBuff,picSize);
        cout<<"Copy picture bytes in buffer. Bytes count: "<<picSize<<endl;
        empPic->setPicData(picBuff);
        msg->getMsg()->setPayload(empPic);
        return msg;

    }else{
        cout<<"Radnik Id: "<<id->getRfid()<< " does not have picture "<<endl;
        string ttmp=picLoc;
        ttmp.append("default.jpg");
        empPic->setPictureName("default.jpg");
        picFile.open(ttmp);
        ifstream picFileD(ttmp);
        int picSize;
        int beg=picFileD.tellg();
        picFileD.seekg(0,ios_base::end);
        picSize=picFileD.tellg();
        picSize=picSize-beg;
        cout<<"Default picture bytes count "<<picSize<<endl;
        picFileD.seekg(ios_base::beg);

        char *picBuff=new char[picSize];
        if(picBuff==NULL){
            return msg;
        }
        empPic->setPictureSize(picSize);
        picFile.read(picBuff,picSize);
        cout<<"Copy picture bytes in buffer. Bytes count: "<<picSize<<endl;
        empPic->setPicData(picBuff);
        msg->getMsg()->setPayload(empPic);
        return msg;

    }
}

InternalMessage *FileProvider::getRadnikActionPicture(InternalMessage *r)
{
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)r->getMsg()->getPayload();
    cout<<"Request for action picture of radnikId: "<<timeInfo->getRfid()->getRfid()<<endl;
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(r->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(r->getMsg()->getHdr().getApiId()));
    msg->getMsg()->getHdr().setTransId(r->getMsg()->getHdr().getTransId());
    EvNetTimePicture *timePic=new EvNetTimePicture;
    timePic->getTime()->setTime(timeInfo->getTime());
    timePic->getTime()->getRfid()->setRfid(timeInfo->getRfid()->getRfid());

    timePic->getPicture()->getRfid()->setRfid(timeInfo->getRfid()->getRfid());


    string tmp=picLoc;
    tmp.append(QString::number(timeInfo->getRfid()->getRfid()).toStdString().c_str());
    tmp.append("_");
    // old func tmp.append(QDateTime::fromMSecsSinceEpoch(QString(r->getTime()).toLong()).toString("dd_MM_yyyy_hh_mm_ss").toStdString());
    tmp.append(QDateTime::fromString(timeInfo->getTime(),DATE_TIME_FORMAT).toString("dd_MM_yyyy_hh_mm_ss").toStdString());
    tmp.append(".jpg");
    timePic->getPicture()->setPictureName(tmp.c_str());
    timePic->getPicture()->setPicType(PictureTypeContainer::JPEG);

    cout<<"Picture location "<<tmp<<endl;
    ifstream picFile(tmp);
    if(picFile.is_open()){
        cout<<timeInfo->getRfid()->getRfid()<<" have picture for time :"<<timeInfo->getTime()<<endl;
        int picSize;
        int beg=picFile.tellg();
        picFile.seekg(0,ios_base::end);
        picSize=picFile.tellg();
        picSize=picSize-beg;
        cout<<"Picture bytes count "<<picSize<<endl;
        timePic->getPicture()->setPictureSize(picSize);
        picFile.seekg(ios_base::beg);
        char *picBuff=new char[picSize];
        if(picBuff==NULL){
            return msg;
        }
        picFile.read(picBuff,picSize);
        cout<<"Copy picture bytes in buffer. Bytes count: "<<picSize<<endl;
        timePic->getPicture()->setPicData(picBuff);
        cout<<"Bytes copiend, cleaning buffer"<<endl;
        msg->getMsg()->setPayload(timePic);
        return msg;

    }else{
        cout<<"radnikId: "<<timeInfo->getRfid()->getRfid()<<" does not have picture for time: "<<timeInfo->getTime()<<endl;
        cout<<"Sending default picture "<<endl;
        timePic->getPicture()->setPictureName(tmp.c_str());
        timePic->getPicture()->setPicType(PictureTypeContainer::JPEG);
        string ttmp=picLoc;
        ttmp.append("default.jpg");
        picFile.open(ttmp);
        ifstream picFileD(ttmp);
        int picSize;
        int beg=picFileD.tellg();
        picFileD.seekg(0,ios_base::end);
        picSize=picFileD.tellg();
        picSize=picSize-beg;
        cout<<"Picture bytes count "<<picSize<<endl;
        picFile.seekg(ios_base::beg);
        char *picBuff=new char[picSize];
        if(picBuff==NULL){
            return msg;
        }
        picFile.read(picBuff,picSize);
        cout<<"Copy picture bytes in buffer. Bytes count: "<<picSize<<endl;
        timePic->getPicture()->setPicData(picBuff);
        cout<<"Bytes copiend, cleaning buffer"<<endl;
        msg->getMsg()->setPayload(timePic);
        return msg;
    }
}

InternalMessage *FileProvider::saveRadnikPicture(InternalMessage *r)
{
    EvNetEmployeePicture *evPic=(EvNetEmployeePicture*)r->getMsg()->getPayload();
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(r->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(r->getMsg()->getHdr().getApiId()));
    msg->getMsg()->getHdr().setTransId(r->getMsg()->getHdr().getTransId());
    EvNetGenericResponse *resp=new EvNetGenericResponse;
    msg->getMsg()->setPayload(resp);

    string tmp=picLoc;
    tmp.append(QString::number(evPic->getRfid()->getRfid()).toStdString().c_str());
    tmp.append(".jpg");
    cout<<"Picture path "<<tmp<<endl;
    ofstream picFile(tmp);
    if(picFile.is_open()){
        cout<<"Saving bytes to "<<tmp<<endl;
        int n=picFile.tellp();
        picFile.write(evPic->getPicData(),evPic->getPictureSize());
        n=picFile.tellp()-n;

        if(n==evPic->getPictureSize()){
            cout<<"Bytes saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            resp->setStatus(GenericResponse::Success);
            return msg;
        }else{
            cout<<"Bytes not saved!!!"<<tmp<<endl;
            resp->setStatus(GenericResponse::Fail);
            resp->setDetail("Bytes not saved!!!");
            resp->setCause(GenericResponseCause::AlreadyExist);
            picFile.flush();
            picFile.close();
            return msg;
        }
    }else{
        resp->setStatus(GenericResponse::Fail);
        string err="Can not open: ";
        err.append(tmp);
        resp->setDetail(err.c_str());
        resp->setCause(GenericResponseCause::AlreadyExist);
        return msg;
    }
    resp->setStatus(GenericResponse::Success);
    return msg;

}

InternalMessage *FileProvider::saveRadnikActionPicture(InternalMessage *r)
{
    EvNetTimePicture *tPic=(EvNetTimePicture *)r->getMsg()->getPayload();
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(r->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(r->getMsg()->getHdr().getApiId()));
    msg->getMsg()->getHdr().setTransId(r->getMsg()->getHdr().getTransId());
    EvNetGenericResponse *resp=new EvNetGenericResponse;
    msg->getMsg()->setPayload(resp);
    cout<<"Save action picture for radnikId: "<<tPic->getPicture()->getRfid()->getRfid()<<endl;
    string tmp=picLoc;
    tmp.append(QString::number(tPic->getPicture()->getRfid()->getRfid()).toStdString().c_str());
    tmp.append("_");
    tmp.append(QDateTime::fromString(tPic->getTime()->getTime(),DATE_TIME_FORMAT).toString("dd_MM_yyyy_hh_mm_ss").toStdString());
    tmp.append(".jpg");
    cout<<"Picture path "<<tmp<<endl;
    ofstream picFile(tmp);
    if(picFile.is_open()){
        cout<<"Saving bytes to "<<tmp<<endl;
        int n=picFile.tellp();
        picFile.write(tPic->getPicture()->getPicData(),tPic->getPicture()->getPictureSize());
        n=picFile.tellp()-n;

        if(n==tPic->getPicture()->getPictureSize()){
            cout<<"Bytes saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            resp->setStatus(GenericResponse::Success);
            return msg;
        }else{
            cout<<"Bytes not saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            resp->setStatus(GenericResponse::Fail);
            resp->setDetail("Bytes not saved!!!");
            resp->setCause(GenericResponseCause::AlreadyExist);
            return msg;
        }
    }else{
        resp->setStatus(GenericResponse::Fail);
        string err="Can not open: ";
        err.append(tmp);
        resp->setDetail(err.c_str());
        resp->setCause(GenericResponseCause::AlreadyExist);
        return msg;
    }
    resp->setStatus(GenericResponse::Success);
    return msg;
}
