#ifndef INTERNALMESSAGE_H
#define INTERNALMESSAGE_H
#include <cstring>

class InternalMessage
{
public:
    InternalMessage();
    int type;
    void setRfif(const int &rid);
    void setCmdType(const int &cmdT);
    void setSenderFd(const int &sFd);
    void setData(const char *lpdata, int iDataSize);

    int getRfid()const;
    int getCmdType()const;
    int getSenderFd()const;
    char *getData();
    const char* getData()const;

private:
    int fd;//sender
    int cmdType;//cmd type
    int rfid;//sender primary key
    char data[4096];
    int dataSize;
};

#endif // INTERNALMESSAGE_H
