#include "fileprovider.h"

FileProvider::FileProvider()
{
    picLoc="/opt/pictures/";
}

InternalMessage *FileProvider::getRadnikPicture(InternalMessage *r)
{
    EvNetEmployeeId *id=(EvNetEmployeeId*)r->getMsg()->getPayload();
    cout<<"Get profile picture for radnikId: "<<id->getRfId()<<endl;
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(r->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(r->getMsg()->getHdr().getApiId()));

    EvNetEmployeePicture *empPic=new EvNetEmployeePicture;
    msg->getMsg()->setPayload(empPic);

    string tmp=picLoc;
    empPic->setRfid(new EvNetEmployeeId(*id));
    empPic->setPicType(PictureTypeContainer::JPEG);

    stringstream ss;
    ss<<id->getRfId();
    string pic;
    ss>>pic;
    tmp.append(pic);
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
        return msg;

    }else{
        cout<<"Radnik Id: "<<id->getRfId()<< " does not have picture "<<endl;
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

        return msg;

    }
}

InternalMessage *FileProvider::getRadnikActionPicture(InternalMessage *r)
{
    EvNetTimeInfo *timeInfo=(EvNetTimeInfo *)r->getMsg()->getPayload();
    cout<<"Request for action picture of radnikId: "<<r->getRadnikId()<<endl;
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setFd(r->getFd());
    msg->setMsg(EvNetMessageBuilder::createMsgWithHdr(r->getMsg()->getHdr().getApiId()));


    string tmp=picLoc;
    stringstream ss;
    ss<<r->getRadnikId();
    string pic;
    ss>>pic;
    tmp.append(pic);
    tmp.append("_");
    tmp.append(QDateTime::fromMSecsSinceEpoch(QString(r->getTime()).toLong()).toString("dd_MM_yyyy_hh_mm_ss").toStdString());
    tmp.append(".jpg");
    cout<<"Picture location "<<tmp<<endl;
    ifstream picFile(tmp);
    if(picFile.is_open()){
        cout<<r->getRadnikId()<<" have picture for time :"<<r->getTime()<<endl;
        int picSize;
        int beg=picFile.tellg();
        picFile.seekg(0,ios_base::end);
        picSize=picFile.tellg();
        picSize=picSize-beg;
        cout<<"Picture bytes count "<<picSize<<endl;
        picFile.seekg(ios_base::beg);
        char *buffer=new char[picSize+sizeof(int)/*id poruke*/+sizeof(long long int)/*radnik id*/+sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke*/];
        char *ptrBuff=buffer;

        int idPoruke=htonl(2);
        memcpy(ptrBuff,&idPoruke,sizeof(idPoruke));
        ptrBuff+=sizeof(idPoruke);

        long long int radnikId=htobe64(r->getRadnikId());
        memcpy(ptrBuff,&radnikId,sizeof(radnikId));
        ptrBuff+=sizeof(radnikId);

        int tipP=htonl(4);
        memcpy(ptrBuff,&tipP,sizeof(tipP));
        ptrBuff+=sizeof(tipP);

        int picS=htonl(picSize);
        memcpy(ptrBuff,&picS,sizeof(picS));
        ptrBuff+=sizeof(picS);

        picFile.read(ptrBuff,picSize);
        cout<<"Copy picture bytes in buffer. Bytes count: "<<picSize<<endl;
        msg->setData(buffer,picSize+sizeof(int)/*id poruke*/+sizeof(long long int)/*radnik id*/+sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke*/);
        cout<<"Bytes copiend, cleaning buffer"<<endl;
        delete [] buffer;
        return msg;

    }else{
        cout<<"radnikId: "<<r->getRadnikId()<<" does not have picture for time: "<<r->getTime()<<endl;
        cout<<"Sending default picture "<<endl;
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
        picFileD.seekg(ios_base::beg);
        char *buffer=new char[picSize+sizeof(int)/*id poruke*/+sizeof(long long int)/*radnik id*/+sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke*/];
        char *ptrBuff=buffer;

        int idPoruke=htonl(2);
        memcpy(ptrBuff,&idPoruke,sizeof(idPoruke));
        ptrBuff+=sizeof(idPoruke);

        long long int radnikId=htobe64(r->getRadnikId());
        memcpy(ptrBuff,&radnikId,sizeof(radnikId));
        ptrBuff+=sizeof(radnikId);

        int tipP=htonl(4);
        memcpy(ptrBuff,&tipP,sizeof(tipP));
        ptrBuff+=sizeof(tipP);

        int picS=htonl(picSize);
        memcpy(ptrBuff,&picS,sizeof(picS));
        ptrBuff+=sizeof(picS);


        picFileD.read(ptrBuff,picSize);
        cout<<"Copy picture bytes in buffer. Bytes count: "<<picSize<<endl;
        msg->setData(buffer,picSize+sizeof(int)/*id poruke*/+sizeof(long long int)/*radnik id*/+sizeof(int)/*tip poruke*/+sizeof(int)/*velicina poruke*/);
        cout<<"Bytes copiend, cleaning buffer"<<endl;
        delete [] buffer;
        return msg;

    }
}

bool FileProvider::saveRadnikPicture(InternalMessage *r)
{

    string tmp=picLoc;
    stringstream ss;
    ss<<r->getRadnikId();
    string pic;
    ss>>pic;
    tmp.append(pic);
    tmp.append(".jpg");
    cout<<"Picture path "<<tmp<<endl;
    ofstream picFile(tmp);
    if(picFile.is_open()){
        cout<<"Saving bytes to "<<tmp<<endl;
        int n=picFile.tellp();
        picFile.write(r->getSlikaData(),r->getSlikaSize());
        n=picFile.tellp()-n;

        if(n==r->getSlikaSize()){
            cout<<"Bytes saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            return true;
        }else{
            cout<<"Bytes not saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            return false;
        }
    }else{
        return false;
    }
    return true;

}

bool FileProvider::saveRadnikActionPicture(InternalMessage *r)
{
    cout<<"Save action picture for radnikId: "<<r->getRadnikId()<<endl;
    string tmp=picLoc;
    stringstream ss;
    ss<<r->getRadnikId();
    string pic;
    ss>>pic;
    tmp.append(pic);
    tmp.append("_");
    tmp.append(QDateTime::fromMSecsSinceEpoch(QString(r->getTime()).toLong()).toString("dd_MM_yyyy_hh_mm_ss").toStdString());
    tmp.append(".jpg");
    cout<<"Picture path "<<tmp<<endl;
    ofstream picFile(tmp);
    if(picFile.is_open()){
        cout<<"Saving bytes to "<<tmp<<endl;
        int n=picFile.tellp();
        picFile.write(r->getSlikaData(),r->getSlikaSize());
        n=picFile.tellp()-n;

        if(n==r->getSlikaSize()){
            cout<<"Bytes saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            return true;
        }else{
            cout<<"Bytes not saved!!!"<<tmp<<endl;
            picFile.flush();
            picFile.close();
            return false;
        }
    }else{
        return false;
    }
    return true;
}
