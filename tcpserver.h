#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <server.h>
#include <sys/select.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <signal.h>
#include <fcntl.h>
#include <cstring>
#include <sys/msg.h>
#include "definicije.h"
#include <internalmessage.h>
#include "fileprovider.h"
#include "messagequeue.h"

using namespace std;


class TcpServer : public Server
{
public:
    TcpServer(MessageQueue *q);
    ~TcpServer();
    void stopServer();
    virtual bool sendDataToClient(const char *data,int dataSize,int socketfd);
private:
    void run();
    typedef void Sigfunc(int);
    void sendToDelegate(char *msg,int msglen);

    Sigfunc *signal(int sig, Sigfunc *f);

    static void sigHandler(int sig);

    void contitionStop();

    static TcpServer* instance;

    bool serverRun;//thread condition
    thread *t1;//obrisan
    mutex m1;

    //select fds
    fd_set readSock,masterRSock;

    //server listen socket
    int sockfd,csock,res;
    unsigned short int m_port;
    int selectReturn;

    //network stuf
    struct sockaddr_in srvaddr,cliaddr;
    char *buffer,*recvbuff,*sndbuff;
    InternalMessage *msg;
    int maxfp1;
    socklen_t clen;
    MessageQueue *queue;

    //network functions
    bool createServer();
    void afterConnect(int fd);
    void clientClose(int fd);
    void parseMsg(char *buf,int bufSize,int fd);
    char bufferSlika[1024*1024*3];


};

#endif // TCPSERVER_H
