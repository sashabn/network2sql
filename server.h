#ifndef SERVER_H
#define SERVER_H
#include "abstractdelegate.h"
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
using namespace std;

class Server
{
public:
    Server();
    void setDelegate(AbstractDelegate *lpabsdelegate);
    static char *getpeerip(int fd);//must free return string from function
protected:
    AbstractDelegate *delegate;
    virtual void clientClose(int fd)=0;
    virtual void conAccept(int fd);
private:
    std::mutex m1;



};

#endif // SERVER_H
