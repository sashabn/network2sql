#include "server.h"

Server::Server()
{
    delegate=NULL;
}

void Server::setDelegate(AbstractDelegate *lpabsdelegate)
{
    m1.lock();
    delegate=lpabsdelegate;
    m1.unlock();
}

char *Server::getpeerip(int fd)
{
    sockaddr_storage cliaddr;
    memset(&cliaddr,'\0',sizeof(cliaddr));

    socklen_t clen;
    clen=sizeof(cliaddr);
    int res=getpeername(fd,(sockaddr*)&cliaddr,&clen);
    if(res==-1){
        cout<<strerror(errno)<<endl;
    }
    char *ipadd=new char[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET,(void*)&((sockaddr_in*)&cliaddr)->sin_addr,ipadd,clen);

    return ipadd;
}

void Server::conAccept(int fd)
{
    char *wrbuf="Connection accept!";
    cout<<"Connection accept on fd: "<<fd<<endl;
     write(fd,wrbuf,strlen(wrbuf));
}
