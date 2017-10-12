#ifndef NETWORKDECODER_H
#define NETWORKDECODER_H
#include "definicije.h"
#include <netinet/in.h>
#include <cstring>
#include <evnetutil.h>

/*

struct{
    int msgType;                        //4byte
    int radnikIdSize;                   //4byte
    unsigned long long radnikId;        //8 byte
    int timeCharCnt;                    //4 byte
    char timeVal[timeCharCnt];          //timeCharCnt byte
    int pictureSize;                    //4 byte
    char pictureData[pictureSize];      //pictureSize byte
};

*/

template <class T>
class NetworkDecoder
{
public:
    NetworkDecoder(int maxMessageSize,T *t,void (T::*callBack)(char *,int,int),int fd){
        this->func=callBack;
        _t=t;
        buffer=new char[maxMessageSize];
        _maxMessageSize=maxMessageSize;
        expectedDataSize=0;
        currentIndexBuffer=0;
        dataSize=0;
        c_fd=fd;
    }

    NetworkDecoder(const NetworkDecoder &rhs)=delete;
    NetworkDecoder()=delete;
    ~NetworkDecoder(){
        delete [] buffer;
    }


    void addBuffer(char *buf,int bufSize){
        if(expectedDataSize==0){
            expectedDataSize=findExpectedDataSize(buf,bufSize);
        }
        if(expectedDataSize>0){
            if((bufSize+currentIndexBuffer)>_maxMessageSize){
                expectedDataSize=0;
                currentIndexBuffer=0;
            }else{
                int bytesFilled=fillMessageBuffer(buf,bufSize);
                if(bytesFilled<bufSize){//there is data left in buffer
                    addBuffer(buf+bytesFilled,bufSize-bytesFilled);
                }
            }
        }
    }





private:
    char *buffer;
    T *_t;
    void (T::*func)(char *buffer,int,int);
    int expectedDataSize;
    int dataSize;
    int currentIndexBuffer;
    int _maxMessageSize;
    int c_fd;

    void sendBuffer(){
        if(func!=NULL&&_t!=NULL){
            (_t->*func)(buffer,dataSize,c_fd);
        }
    }

    int readOnePayloadMessageSize(char *buf,int bufByteSize){
        int readSize=0;
        if(bufByteSize<20){
            return -1;
        }
        readSize=htonl(*((int*)(buf+16)));
        readSize+=20;
        return readSize;

    }

    int readTwoPayloadMessageSize(char *buf,int bufByteSize){
        int readSize=readOnePayloadMessageSize(buf,bufByteSize);
        if(readSize==-1){
            return -1;
        }
        if(bufByteSize<(readSize+4)){
            return -1;
        }
        readSize+=htonl(*((int*)(buf+readSize)));
        readSize+=4;
        return readSize;
    }

    int findExpectedDataSize(char *buf,int bufByteSize){
        int expDataSize=EvNetUtil::fromByteArray<int>(buf,4,true);
        cout<<"Expected data size: "<<expDataSize<<endl;
        return expDataSize;
    }

    int fillMessageBuffer(char *buf,int bufByteSize){
        int bufToRead=(expectedDataSize>bufByteSize)?bufByteSize:expectedDataSize;
        memcpy(buffer+currentIndexBuffer,buf,bufToRead);
        expectedDataSize-=bufToRead;
        currentIndexBuffer+=bufToRead;
        if(expectedDataSize==0){
            (_t->*func)(buffer,currentIndexBuffer,this->c_fd);
            currentIndexBuffer=0;

        }
        return bufToRead;
    }
};

#endif // NETWORKDECODER_H
