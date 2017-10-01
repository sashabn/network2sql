#include "tcpserver.h"
TcpServer* TcpServer::instance=NULL;

TcpServer::TcpServer(MessageQueue *q):queue(q)
{

    sockfd=-1;
    msg=NULL;
    t1=NULL;
    m_port=1929;
    selectReturn=0;
    maxfp1=0;
    serverRun=true;
    instance=this;
    buffer=new char[BUFSIZE];
    recvbuff=new char[BUFSIZE];
    sndbuff=new char[BUFSIZE];
    if(!createServer()){
        //greska
        cout<<"ERROR create server";

        return;
    }else{
        cout<<"Starting server on port "<<m_port<<endl;
    }
    t1=new thread(&TcpServer::run,this);
    cout<<"Thread is running"<<endl;

}

TcpServer::~TcpServer()
{
    serverRun=false;
    if(t1!=NULL){
        if(t1->joinable()){
            t1->join();
            delete t1;
            cout<<"SOFT SHUTDOWN"<<endl;
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
                continue;
            }
            cout<<strerror(errno)<<" "<<"select error"<<endl;
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
                    cout<<"Accept connection on fd: "<<csock<<endl;
                    if(csock==-1){
                        if(errno==EINTR)
                            goto ponovo;
                        cout<<
                               rsize--;
                        continue;
                    }
                    FD_SET(csock,&masterRSock);
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
                    if(fdBufferMap[i]!=NULL){
                        fdBufferMap[i]->addBuffer(buffer,res);
                    }
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
    cout<<"Alocating buffer for fd: "<<fd<<endl;
    fdBufferMap[fd]=new NetworkDecoder<TcpServer>(1024*1024*3,this,&TcpServer::parseMsg,fd);
    int sendId[2]={1,fd};
    char *r=Server::getpeerip(fd);
    cout<<"Peer connected from ip "<<r<<endl;
    m1.lock();
    msg=new InternalMessage();//treba obrisati negde
    msg->setSenderFd(fd);
    msg->setCmdType(1);
    msg->setData(r,strlen(r));
    delete [] r;
    queue->addMessage(msg);
    m1.unlock();

pocetak:
    int sread=read(fd,buffer,BUFSIZE);
    if(sread==0){//konekcija je zavrsena
        clientClose(fd);
    }
    if(sread<0){
        if(errno==EAGAIN||errno==EINTR){
            goto pocetak;
        }
        this->clientClose(fd);
        return;
    }

    if((res=stringF(buffer,sread,"+OK",3))!=0){
        cout<<"Unknow protocol!!"<<endl;
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
    cout<<"Client "<<r<<" disconnected!!"<<" "<<fd<<endl;
    delete [] r;
    FD_CLR(fd,&masterRSock);
    FD_CLR(fd,&readSock);
    if(fdBufferMap[fd]!=NULL){
        delete fdBufferMap[fd];
    }
    fdBufferMap[fd]=NULL;
    close(fd);
}

void TcpServer::parseMsg(char *buf, int bufSize,int fd)
{
    m1.lock();
    cout<<"Parsing message from fd "<<fd<<endl;
    msg=new InternalMessage();//treba obrisati negde
    EvNetMessage *netMsg=EvNetMessageBuilder::createMessageFromBytes(buf,bufSize,true);
    msg->setMsg(netMsg);
    msg->setFd(fd);
    queue->addMessage(msg);
    m1.unlock();
}

void TcpServer::testF(char *test, int size)
{
    write(2,test,size);
}


bool TcpServer::sendDataToClient(const char *data, int dataSize, int socketfd)
{
    cout<<"Sending data size : "<<dataSize<<" to fd: "<<socketfd<<endl;
    const char *ptrBuf=data;
    int bytesLeft=dataSize;
    int bytesSend=0;
    int n;
    while(bytesLeft!=0){
        n=write(socketfd,ptrBuf+bytesSend,bytesLeft);
        if(n==0){
            clientClose(socketfd);
            bytesLeft=0;
            return false;
        }
        if(n<0){
            if(errno==EINTR)
                continue;
            std::cout<<strerror(errno)<<std::endl;
            return false;
        }
        bytesSend+=n;
        bytesLeft-=n;
    }
    cout<<"Data sent success to fd: "<<socketfd<<endl;
    return true;
}
