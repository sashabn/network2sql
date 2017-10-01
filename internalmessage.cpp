#include "internalmessage.h"

InternalMessage::InternalMessage()
{
    fd=0;
}

InternalMessage::~InternalMessage()
{

}

InternalMessage::InternalMessage(const InternalMessage &rhs)
{
    fd=rhs.fd;
    msg=rhs.msg;
}

EvNetMessage *InternalMessage::getMsg()
{
    return msg;
}

void InternalMessage::setMsg(EvNetMessage *value)
{
    msg = value;
}

int InternalMessage::getFd() const
{
    return fd;
}

void InternalMessage::setFd(int value)
{
    fd = value;
}

int InternalMessage::getCmdType() const
{
    return cmdType;
}

void InternalMessage::setCmdType(int value)
{
    cmdType = value;
}

void InternalMessage::setRfif(const long long int &rid)
{
    rfid=rid;
}

