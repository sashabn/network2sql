#ifndef DEFINICIJE_H
#define DEFINICIJE_H

#define CENTRAL_MAILBOX 1200
struct message {
    long type;
    char *msg;
};
#define BUFSIZE 4096

#define CLIENT_CONNECTED 1
#define CLIENT_SEND_MESSAGE 2

#define QUERY_GET_MATICNI_INFO 1
#define QUERT_GET_PICTURE 2
#include <iomanip>
#include <cstring>
#include <iostream>
extern int maxLineSize;
extern int getLineSize(char *line,int num);
extern int getIntNumCnt(int num);
using std::cout;
using std::endl;


#define cout cout<<"log in file "<<__FILE__<<" at line "<<__LINE__<<std::right <<std::setw(getLineSize(__FILE__,__LINE__))<<" "





inline int stringF(char *s1, int ls1, char *s2, int ls2)
{
    if(ls1<ls2){
        return -1;
    }
    for(int i=0;i<ls1;i++){
        if((ls1-ls2)>=i){
            for(int j=0;j<ls2;j++){
                if(s1[i+j]!=s2[j]){
                    break;
                }
                if(j==(ls2-1))
                    return i;
            }
        }
    }
    return -1;
}

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


#endif // DEFINICIJE_H
