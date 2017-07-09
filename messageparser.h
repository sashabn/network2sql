#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H
#include <netinet/in.h>
#include <cstring>
#include <iostream>

using namespace std;

enum MessageType{RequestStatus=1,
                RequestRadnikInfo,
                RequestRadnikPicture,
                Ulaz,
                Izlaz,
                TerenPocetak,
                TerenKraj,
                PauzaPocetak,
                PauzaKraj,
                PrivatnoPocetak,
                PrivatnoKraj,
                UpdateSlika,
                ActionSlika,
                RequestActionSlika};
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
