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

    InternalMessage *msg=NULL;
    MessageQueue *queue=new MessageQueue();
    cout<<"PROSO"<<endl;
    MessageAsyncProccessor *mProcess=new MessageAsyncProccessor(queue);
    TcpServer *s=new TcpServer(queue);
    cout<<"PROSO"<<endl;

    while(running){
        int rsize;
        msg=queue->getNextMessage();
        if(msg==NULL)
            continue;
        int type=msg->getCmdType();
        MessageParser *parsedMsg=NULL;
        switch (type) {
        case 1://new connection
            cout<<"Konekcija je uspostavljena sa "<<msg->getData()<<" na socket descriptoru "<<msg->getSenderFd()<<endl;
            break;
        case 2://got message from client
            parsedMsg=new MessageParser(msg->getData(),msg->getDataSize(),msg->getSenderFd());
            mProcess->addMessage(parsedMsg);
            cout<<"PORUKA DOLAZI SA "<<msg->getSenderFd()<<endl;
            cout<<"REQUEST "<<parsedMsg->getRadnikId()<<endl;
            break;
        case 3://send message to client
            cout<<"OVO JE PORUKA ZA NETWORK bajti"<<msg->getDataSize()<<endl;
            if(s->sendDataToClient(msg->getData(),msg->getDataSize(),msg->getSenderFd()))
                cout<<"Podaci klijentu su uspjesno poslati"<<endl;
            break;
        }
         delete msg;
    }
    cout<<"PROSO"<<endl;
    s->stopServer();
    cout<<"PROSO"<<endl;
    delete s;
    delete mProcess;
    return 1;

}
