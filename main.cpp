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
#include <messageparser.h>
#include <messageasyncproccessor.h>

int maxLineSize=30;

typedef void Sigfunc(int);


int getLineSize(char *line,int num){
    int lineSize=strlen(line);
    lineSize+=getIntNumCnt(num);
    if(maxLineSize<(lineSize+4)){
        maxLineSize=lineSize+4;
    }
    return maxLineSize-lineSize;
}

int getIntNumCnt(int num){
    int numCnt=0;
    while(num){
        num/=10;
        numCnt++;
    }
    return numCnt;
}

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

    InternalMessage *msg=NULL;
    MessageQueue *queue=new MessageQueue();
    MessageAsyncProccessor *mProcess=new MessageAsyncProccessor(queue);
    TcpServer *s=new TcpServer(queue);
    cout<<"Server is up and running !!!"<<endl;

    while(running){
        int rsize;
        msg=queue->getNextMessage();
        if(msg==NULL)
            continue;
        int type=msg->getCmdType();
        MessageParser *parsedMsg=NULL;
        switch (type) {
        case 1://new connection
            cout<<"Connection establish from: "<<msg->getData()<<" with socket fd: "<<msg->getSenderFd()<<endl;
            break;
        case 2://got message from client
            parsedMsg=new MessageParser(msg->getData(),msg->getDataSize(),msg->getSenderFd());
            mProcess->addMessage(parsedMsg);
            cout<<"Message comes from socket fd: "<<msg->getSenderFd()<<endl;
            cout<<"radnikId: "<<parsedMsg->getRadnikId()<<endl;
            break;
        case 3://send message to client
            cout<<"Prepering to send message bytes: "<<msg->getDataSize()<<" to network client on socket fd: "<<msg->getSenderFd()<<endl;
            if(s->sendDataToClient(msg->getData(),msg->getDataSize(),msg->getSenderFd()))
                cout<<"Data size: "<<msg->getDataSize()<<"successfully sent to client with socket fd: "<<msg->getSenderFd()<<endl;
            break;
        }
         delete msg;
    }
    s->stopServer();
    delete s;
    delete mProcess;
    return 1;

}
