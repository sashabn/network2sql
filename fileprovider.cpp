#include "fileprovider.h"

FileProvider::FileProvider()
{
    picLoc="/opt/pictures/";
}

InternalMessage *FileProvider::getRadnikPicture(MessageParser *r)
{
    cout<<"Get profile picture for radnikId: "<<r->getRadnikId()<<endl;
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setRfif(r->getRadnikId());
    msg->setSenderFd(r->getSenderFd());

    string tmp=picLoc;
    stringstream ss;
    ss<<r->getRadnikId();
    string pic;
    ss>>pic;
    tmp.append(pic);
    tmp.append(".jpg");
    cout<<"Picture location "<<tmp<<endl;
    ifstream picFile(tmp);
    if(picFile.is_open()){
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
        cout<<"Radnik Id: "<<r->getRadnikId()<< " does not have picture "<<endl;
        string ttmp=picLoc;
        ttmp.append("default.jpg");
        picFile.open(ttmp);
        ifstream picFileD(ttmp);
        int picSize;
        int beg=picFileD.tellg();
        picFileD.seekg(0,ios_base::end);
        picSize=picFileD.tellg();
        picSize=picSize-beg;
        cout<<"Default picture bytes count "<<picSize<<endl;
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

InternalMessage *FileProvider::getRadnikActionPicture(MessageParser *r)
{
    cout<<"Request for action picture of radnikId: "<<r->getRadnikId()<<endl;
    InternalMessage *msg=new InternalMessage;
    msg->setCmdType(3);
    msg->setRfif(r->getRadnikId());
    msg->setSenderFd(r->getSenderFd());

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

bool FileProvider::saveRadnikPicture(MessageParser *r)
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

bool FileProvider::saveRadnikActionPicture(MessageParser *r)
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
