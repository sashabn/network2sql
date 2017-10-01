#ifndef INTERNALMESSAGE_H
#define INTERNALMESSAGE_H
#include <cstring>
#include <iostream>
#include "definicije.h"
#include <evnetmessage.h>

class InternalMessage
{
public:
    InternalMessage();
    ~InternalMessage();
    InternalMessage(const InternalMessage &rhs);

    EvNetMessage *getMsg();
    void setMsg(EvNetMessage *value);

    int getFd() const;
    void setFd(int value);

    int getCmdType() const;
    void setCmdType(int value);

private:
    int fd;//sender
    EvNetMessage *msg;
    int cmdType;
};

#endif // INTERNALMESSAGE_H
