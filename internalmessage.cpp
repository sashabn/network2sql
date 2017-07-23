#include "internalmessage.h"

InternalMessage::InternalMessage()
{

    type=1;
    fd=0;
    cmdType=0;
    rfid=0;
    dataSize=0;
    data=NULL;
    cout<<"Internal message created!!"<<endl;
}

InternalMessage::~InternalMessage()
{
    if(data!=NULL){
        delete [] data;
    }
    cout<<"Internal message destroyed !!"<<endl;
}

InternalMessage::InternalMessage(const InternalMessage &rhs)
{
    cout<<"Internal message created!!"<<endl;
    this->type=rhs.type;
    this->fd=rhs.fd;
    this->cmdType=rhs.cmdType;
    this->rfid=rhs.rfid;
    this->dataSize=rhs.dataSize;
    this->data=new char(this->dataSize);
    memcpy(this->data,rhs.data,this->dataSize);

}

void InternalMessage::setRfif(const long long int &rid)
{
    rfid=rid;
}

void InternalMessage::setCmdType(const int &cmdT)
{
    cmdType=cmdT;
}

void InternalMessage::setSenderFd(const int &sFd)
{
    fd=sFd;
}

void InternalMessage::setData(const char *lpdata, int iDataSize)
{
    if(this->data!=NULL){
        delete [] this->data;
        this->data=NULL;
        iDataSize=0;
    }
    this->data=new char[iDataSize];
    if(this->data==NULL){

        std::cout<<"Data is NULL for fd :"<<fd<<std::endl;
        return;
    }
    dataSize=iDataSize;
    memcpy(this->data,lpdata,iDataSize);
}


long long int InternalMessage::getRfid() const
{
    return rfid;
}

int InternalMessage::getCmdType() const
{
    return cmdType;
}

int InternalMessage::getSenderFd() const
{
    return fd;
}

//char *InternalMessage::getData()
//{
//    return this->data;
//}

const char *InternalMessage::getData() const
{
    return (const char*)this->data;
}

int InternalMessage::getDataSize() const
{
    return dataSize;
}

