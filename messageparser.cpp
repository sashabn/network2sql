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
        cout<<"Message type "<<type<<endl;
        switch (mType) {
        case MessageType::RequestStatus:
        case MessageType::RequestRadnikInfo:
        case MessageType::RequestRadnikPicture:
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//id radnik
            ptrSize+=sizeof(long long int);
            cout<<"Request Status/RadnikInfo/RadnikPicture radnik id: "<<msgVal<<endl;
            break;
        case MessageType::Ulaz:
        case MessageType::Izlaz:
        case MessageType::TerenPocetak:
        case MessageType::TerenKraj:
        case MessageType::PauzaPocetak:
        case MessageType::PauzaKraj:
        case MessageType::PrivatnoPocetak:
        case MessageType::PrivatnoKraj:
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//idradnik
            cout<<"Request Ulaz/Izlaz/TerenPocetak/TerenKraj/PauzaPocetak/PauzaKraj/PrivatnoPocetak/PrivatnoKraj radnik id: "<<msgVal<<endl;
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            timeValSize=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            timeVal=new char[timeValSize+1];
            memcpy(timeVal,ptr,timeValSize);
            ptrSize+=timeValSize;
            timeVal[timeValSize]='\0';
            cout<<"Time in sec "<<timeVal<<" for radnik id: "<<msgVal<<endl;
            break;
        case MessageType::UpdateSlika:
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//idradnik
            cout<<"Request UpdateSlika radnik id "<<msgVal<<endl;
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            slikaSize=ntohl(*((int*)ptr));
            cout<<"Picture size "<<slikaSize<<endl;
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            slikaData=new char[slikaSize];
            memcpy(slikaData,ptr,slikaSize);
            ptrSize+=slikaSize;

            break;
        case MessageType::ActionSlika:
            sizeMsg=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            msgVal=be64toh(*((long long int*)ptr));//idradnik
            cout<<"Request Action Picture radnik id "<<msgVal<<endl;
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            timeValSize=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            timeVal=new char[timeValSize+1];
            memcpy(timeVal,ptr,timeValSize);
            timeVal[timeValSize]='\0';
            ptr+=timeValSize;
            ptrSize+=timeValSize;
            slikaSize=ntohl(*((int*)ptr));
            cout<<"Picture size "<<slikaSize<<endl;
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
            cout<<"Request GET Action Picture radnik id "<<msgVal<<endl;
            cout<<msgVal<<endl;
            ptr+=sizeof(msgVal);
            ptrSize+=sizeof(msgVal);
            timeValSize=ntohl(*((int*)ptr));
            ptr+=sizeof(int);
            ptrSize+=sizeof(int);
            timeVal=new char[timeValSize+1];
            memcpy(timeVal,ptr,timeValSize);
            ptrSize+=timeValSize;
            timeVal[timeValSize]='\0';
            break;

        }
        if(ptrSize!=dataSize){
            cout<<"Strange error "<<ptrSize<<" "<<dataSize<<std::endl;
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
