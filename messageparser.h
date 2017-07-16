#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include "definicije.h"

using namespace std;


class MessageParser
{
public:
    MessageParser(const char *data, const int &dataSize,int sender);
    ~MessageParser();
    const MessageType &getMessageType()const;
    long long int getRadnikId()const;
    const char* getTime()const;
    const char* getSlikaData()const;
    int getSlikaSize()const;
    int getSenderFd()const;
private:
    int fd;//sender
    MessageType mType;
    long long int radnikId;
    char *timeValue;
    char *slikaData;
    int slikaSize;
};



#endif // MESSAGEPARSER_H
