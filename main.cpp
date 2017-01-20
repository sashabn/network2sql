#include <iostream>
#include <tcpserver.h>
#include <sys/msg.h>
#include "definicije.h"
#include <sys/signal.h>
#include <gdate.h>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>
#include <internalmessage.h>

typedef void Sigfunc(int);

bool running;
Sigfunc* signal(int sig, Sigfunc *handler){
    struct sigaction act,oact;
    act.sa_handler=handler;
    act.sa_flags=0;
    if(sig==SIGALRM){
        act.sa_flags|=SA_INTERRUPT;
    }else{
        act.sa_flags|=SA_RESTART;
    }
    sigemptyset(&act.sa_mask);
    if(sigaction(sig,&act,&oact)<0){
        return SIG_ERR;
    }
    return oact.sa_handler;
}

void sighndl(int sig){
    std::cout<<"OCE QRAC"<<std::endl;
    running=false;
}

using namespace std;

int main(int argc, char *argv[])
{
    signal(SIGINT,&sighndl);
    signal(SIGUSR1,&sighndl);
    running=true;
    InternalMessage *msg=new InternalMessage;
    cout<<"PROSO"<<endl;
    msqidC = msgget(IPC_PRIVATE, 0600|IPC_CREAT|IPC_EXCL);
    TcpServer *s=new TcpServer;
    s->setMsgId(msqidC);
    cout<<"PROSO"<<endl;

    cout<<msqidC<<endl;

    while(running){
        int rsize;
        rsize=msgrcv(msqidC,msg,sizeof(InternalMessage),0,0);
        if(rsize<0){
            if(errno==EINTR){
                cout<<"PONOVO AJD"<<endl;
                continue;
            }
            cout<<strerror(errno)<<endl;
            return -1;
        }

        cout<<"Poruku salje1 "<<msg->getSenderFd()<<endl;
        //cout<<*id<<endl;//tip poruke
        //cout<<*++id<<endl;// socket

    }
    cout<<"PROSO"<<endl;
    s->stopServer();
    cout<<"PROSO"<<endl;
    delete s;
    return 1;

}
