#include "internalmessage.h"

InternalMessage::InternalMessage()
{

    type=1;
    fd=0;
    cmdType=0;
    rfid=0;
    dataSize=0;
    data=NULL;
}

InternalMessage::~InternalMessage()
{

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
    if(this->data!=NULL)
        delete this->data;
    this->data=new char[iDataSize];
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

char *InternalMessage::getData()
{
    return this->data;
}

const char *InternalMessage::getData() const
{
    return (const char*)this->data;
}

int InternalMessage::getDataSize() const
{
    return dataSize;
}

void InternalMessage::deleteData()
{
    delete [] data;
}
