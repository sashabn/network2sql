#include "internalmessage.h"

InternalMessage::InternalMessage()
{

    type=1;
    fd=0;
    cmdType=0;
    rfid=0;
    memset(data,'\0',sizeof(data));
    dataSize=0;
}

void InternalMessage::setRfif(const int &rid)
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
    if(iDataSize>4096){
        return;
    }
    dataSize=iDataSize;
    memcpy(this->data,lpdata,iDataSize);
}


int InternalMessage::getRfid() const
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
