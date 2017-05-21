#ifndef DATABASE_H
#define DATABASE_H
class InternalMessage;
class MessageParser;

class Database
{
public:
    Database();
    virtual InternalMessage *getResult(MessageParser *p)=0;
};

#endif // DATABASE_H
