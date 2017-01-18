#include "tcpserver.h"
TcpServer* TcpServer::instance=NULL;

TcpServer::TcpServer()
{
    sockfd=-1;
    t1=NULL;
    m_port=1929;
    selectReturn=0;
    maxfp1=0;
    serverRun=true;
    instance=this;
    buffer=new char[BUFSIZE];
    recvbuff==new char[BUFSIZE];
    sndbuff=new char[BUFSIZE];
    cout<<"TEST"<<msqidC<<endl;
    if(!createServer()){
        //greska
        cout<<"GRESKA";

        return;
    }else{
        cout<<"Starting server on port "<<m_port<<endl;
    }
    t1=new thread(&TcpServer::run,this);
    cout<<"KRAJ"<<endl;

}

TcpServer::~TcpServer()
{
    serverRun=false;
    if(t1!=NULL){
        if(t1->joinable()){
            t1->join();
            delete t1;
            cout<<"JEST JEST"<<endl;
        }
    }
}

void TcpServer::stopServer()
{
    if(t1!=NULL)
        pthread_kill(t1->native_handle(),SIGUSR1);
    this->contitionStop();
}

void TcpServer::run()
{
    for(;serverRun;){
        readSock=masterRSock;
        selectReturn=select(maxfp1,&readSock,NULL,NULL,NULL);
        if(selectReturn<0){
            if(errno==EINTR){
                cout<<"A OVDE"<<endl;
                continue;
            }
            cout<<strerror(errno)<<" "<<"select"<<endl;
            serverRun=false;
            continue;
        }
        int rsize=selectReturn;
        for(int i=0;i<maxfp1;i++){
            if(rsize<=0)
                continue;
            if(FD_ISSET(i,&readSock)){
                if(i==sockfd){
                    ponovo:
                    csock=accept(i,(sockaddr*)&cliaddr,&clen);
                    if(csock==-1){
                        if(errno==EINTR)
                            goto ponovo;
                        cout<<
                        rsize--;
                        continue;
                    }
                    FD_SET(csock,&masterRSock);
//                    maxfp1=max(maxfp1,csock)+1;
                    maxfp1=(maxfp1>csock)?maxfp1:(csock+1);
                    cout<<"maxfp1 je "<<maxfp1<<endl;
                    afterConnect(csock);
                    
                }else{//konekcija dolazi sa klijenta
                    ponovoRead:
                    res=read(i,buffer,BUFSIZE);
                    if(res<0){
                        if(errno==EINTR){
                            goto ponovoRead;
                        }
                        rsize--;
                        clientClose(i);
                        continue;
                    }
                    if(res==0){
                        clientClose(i);
                        rsize--;
                        continue;
                    }
                    parseMsg(buffer,res,i);
                    rsize--;
                    continue;

                }
            }
        }
        
        
    }
}

void TcpServer::sendToDelegate(char *msg, int msglen)
{
    if(delegate!=NULL){
        if(!delegate->proccess(msg,msglen)){
            cout<<"Unable to process message in delegate"<<endl;
            return;
        }
    }
}

TcpServer::Sigfunc *TcpServer::signal(int sig, TcpServer::Sigfunc *f)
{
    struct sigaction act,oldact;
    sigemptyset(&act.sa_mask);
    act.sa_handler=f;
    act.sa_flags=0;
    if(sig==SIGALRM){
#ifdef SA_INTERRUPT
    act.sa_flags|=SA_INTERRUPT;
#endif
    }else{
#ifdef SA_RESTART
        act.sa_flags|=SA_RESTART;
#endif
    }
    if(sigaction(sig,&act,&oldact)<0){
        return SIG_ERR;
    }
    return oldact.sa_handler;

}

void TcpServer::sigHandler(int sig)
{
   if(sig==SIGUSR1){
       cout<<"SIGNAL STOP"<<endl;;
        TcpServer::instance->contitionStop();
   }
}

void TcpServer::contitionStop()
{
    serverRun=false;
}

bool TcpServer::createServer()
{
    memset(&srvaddr,'\0',sizeof(srvaddr));
    srvaddr.sin_port=htons(m_port);
    srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    srvaddr.sin_family=AF_INET;
    sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    int reuse=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
    if(sockfd<0){
        //greska
        cout<<strerror(errno)<<endl;
        return false;
    }
    if((res=bind(sockfd,(sockaddr*)&srvaddr,sizeof(srvaddr)))<0){
        //greska
        cout<<strerror(errno)<<endl;
        return false;
    }
    if((res=listen(sockfd,10))<0){
        //greska
        cout<<strerror(errno)<<endl;
        return false;
    }
    FD_SET(sockfd,&masterRSock);
    maxfp1=max(sockfd,maxfp1)+1;
    return true;
}

void TcpServer::afterConnect(int fd)
{
    int sendId[2]={1,fd};
    memcpy(msg.text,sendId,sizeof(sendId));
    msg.type=1;
    msgsnd(msqidC,&msg,sizeof(sendId),0);
    cout<<"Konekcija dolazi sa ";
    char *r=Server::getpeerip(fd);
    cout<<r<<" "<<fd<<endl;
    delete r;
    r=0;
    pocetak:
    int sread=read(fd,buffer,BUFSIZE);
    write(fd,buffer,sread);
    if(sread==0){//konekcija je zavrsena
        close(fd);
        FD_CLR(fd,&masterRSock);
    }
    if(sread<0){
        if(errno==EAGAIN||errno==EINTR){
            goto pocetak;
        }
        this->clientClose(fd);
        return;
    }

    if((res=stringF(buffer,sread,"+OK",3))!=0){
        cout<<res<<" OVO NIJE PROSAO"<<(int)buffer[0]<<" "<<(int)'+'<<" "<<sread<<endl;
        this->clientClose(fd);
    }else{
        this->conAccept(fd);
    }
    buffer[sread]='\0';
    cout<<buffer<<endl;


}

void TcpServer::clientClose(int fd)
{
    char *r=Server::getpeerip(fd);
    cout<<"Client "<<r<<" was disconnected!!"<<" "<<fd<<endl;
    delete r;
    FD_CLR(fd,&masterRSock);
    FD_CLR(fd,&readSock);
    close(fd);
}

void TcpServer::parseMsg(char *buf, int bufSize,int fd)
{


    char *r;
    int sendId[2]={2,fd};

    memcpy(msg.text,sendId,sizeof(sendId));
    msgsnd(msqidC,&msg,sizeof(sendId),0);
    if(delegate!=NULL){
        memcpy(recvbuff,buf,bufSize);
        delegate->decrypt(recvbuff,bufSize);
    }
    r=Server::getpeerip(fd);

    char *ptr=buf;
    int type=ntohl(*((int*)ptr));
    ptr+=sizeof(int);
    int sizeMsg;
    int msgVal;

    switch (type) {
    case 1:
        sizeMsg=ntohl(*((int*)ptr));
        ptr+=sizeMsg;
        msgVal=ntohl(*((int*)ptr));
        break;
    }
    cout<<"Poruka sa clienta "<<r<<" "<<msgVal<<" "<<(int)buf[0]<<endl;
    write(fd,buf,bufSize);



    delete r;
}

void TcpServer::setMsgId(int id)
{
    msqidC=id;
}
