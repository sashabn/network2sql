#ifndef INTERNALMESSAGE_H
#define INTERNALMESSAGE_H
#include <cstring>
#include <iostream>

class InternalMessage
{
public:
    InternalMessage();
    ~InternalMessage();
    InternalMessage(const InternalMessage &rhs);
    int type;
    void setRfif(const long long int &rid);
    void setCmdType(const int &cmdT);
    void setSenderFd(const int &sFd);
    void setData(const char *lpdata, int iDataSize);

    long long int getRfid()const;
    int getCmdType()const;
    int getSenderFd()const;
//    const char *getData();
    const char* getData()const;
    int getDataSize()const;
private:
    int fd;//sender
    int cmdType;//cmd type
    long long int rfid;//sender primary key
    char *data;
    int dataSize;
};

#endif // INTERNALMESSAGE_H
