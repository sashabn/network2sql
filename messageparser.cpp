#include "messageparser.h"



MessageParser::MessageParser(const char *data, const int &dataSize, int sender):fd(sender)
{
        const char *ptr=data;
        int type=ntohl(*((int*)ptr));
        int ptrSize=0;

        ptr+=sizeof(int);
        ptrSize+=sizeof(int);
        int sizeMsg;
        long long int msgVal;
        char *timeVal=NULL;
        int timeValSize;
        slikaSize=0;
        slikaData=NULL;
        mType=(MessageType)type;
        cout<<"TIP PORUKE JE "<<type<<endl;
        switch (mType) {
        case MessageType::RequestStatus:
        case MessageType::RequestRadnikInfo:
        case MessageType::RequestRadnikPicture:
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//id radnik
            ptrSize+=sizeof(long long int);
            cout<<msgVal<<" "<<"ID RADNIK"<<endl;
            break;
        case MessageType::Ulaz:
        case MessageType::Izlaz:
        case MessageType::TerenPocetak:
        case MessageType::TerenKraj:
        case MessageType::PauzaPocetak:
        case MessageType::PauzaKraj:
        case MessageType::PrivatnoPocetak:
        case MessageType::PrivatnoKraj:
            cout<<"DOSAO JE DO MENE "<<endl;
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//idradnik
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            timeValSize=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            timeVal=new char[timeValSize+1];
            memcpy(timeVal,ptr,timeValSize);
            ptrSize+=timeValSize;
            timeVal[timeValSize]='\0';
            cout<<timeVal<<" OVO JE VREME U SEC"<<endl;
            break;
        case MessageType::UpdateSlika:
            sizeMsg=ntohl(*((int*)ptr));
            cout<<"ID RADNIK UZIMA "<<sizeMsg<<endl;
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//idradnik
            cout<<msgVal<<" OVO JE RADNIK "<<endl;
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            slikaSize=ntohl(*((int*)ptr));
            cout<<"SLIKA SIZE JE "<<slikaSize<<endl;
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            slikaData=new char[slikaSize];
            memcpy(slikaData,ptr,slikaSize);
            ptrSize+=slikaSize;

            break;
        case MessageType::ActionSlika:
            cout<<"DADA";
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//idradnik
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            timeValSize=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            cout<<"DADA";
            timeVal=new char[timeValSize+1];
            memcpy(timeVal,ptr,timeValSize);
            timeVal[timeValSize]='\0';
            ptr+=timeValSize;
            ptrSize+=timeValSize;
            slikaSize=ntohl(*((int*)ptr));
            cout<<"SLIKA SIZE JE "<<slikaSize<<endl;
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            slikaData=new char[slikaSize];
            memcpy(slikaData,ptr,slikaSize);
            ptrSize+=slikaSize;
            break;
        case MessageType::RequestActionSlika:
            sizeMsg=ntohl(*((int*)ptr));
            cout<<sizeMsg<<endl;
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//id radnik
            cout<<msgVal<<endl;
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            timeValSize=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            cout<<"DADA";
            timeVal=new char[timeValSize+1];
            memcpy(timeVal,ptr,timeValSize);
            ptrSize+=timeValSize;
            timeVal[timeValSize]='\0';
            break;

        }
        if(ptrSize!=dataSize){
            std::cout<<"OVO BI TREBALO PROVERITI "<<ptrSize<<" "<<dataSize<<std::endl;
        }
        radnikId=msgVal;
        timeValue=timeVal;

}

MessageParser::~MessageParser()
{
    if(timeValue!=NULL)
        delete [] timeValue;
    if(slikaData!=NULL)
        delete [] slikaData;
}

const MessageType &MessageParser::getMessageType() const
{
    return mType;
}

long long int MessageParser::getRadnikId() const
{
    return radnikId;
}

const char *MessageParser::getTime() const
{
    return timeValue;
}

const char *MessageParser::getSlikaData() const
{
    return slikaData;
}

int MessageParser::getSlikaSize() const
{
    return slikaSize;
}

int MessageParser::getSenderFd() const
{
    return fd;
}
