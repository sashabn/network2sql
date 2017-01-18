#include "gdate.h"

GDate::GDate()
{
    my_tm=NULL;
    my_tm=new tm;
}

GDate::GDate(time_t epoch)
{
    my_tm=NULL;
    my_tm=localtime((const time_t*)&epoch);
    clearTm(my_tm);
    epochTime=mktime(my_tm);
}

GDate *GDate::fromString(const char *date, const char *format)
{
    GDate *tmpObj=new GDate();
    stringstream ss(date);
    if(ss>>get_time(tmpObj->my_tm,format)){
        tmpObj->epochTime=mktime(tmpObj->my_tm);
        return tmpObj;
    }else{
        cout<<strerror(errno)<<endl;
        return NULL;
    }
}

const char *GDate::toString(const char *format)
{
    if(format==NULL){
        char spacer='-';
        if(this->my_tm!=NULL){
        char *year;
        int size=int2char(this->my_tm->tm_year+1900,&year);
        char *month;
        size+=int2char(this->my_tm->tm_mon+1,&month);
        char *day;
        size+=int2char(this->my_tm->tm_mday,&day);
        char *buf=new char[size+3];
        char *ptrbuf=buf;
        strcpy(ptrbuf,year);
        ptrbuf[strlen(year)]=spacer;
        ptrbuf+=strlen(year)+1;

        strcpy(ptrbuf,month);
        ptrbuf[strlen(month)]=spacer;
        ptrbuf+=strlen(month)+1;

        strcpy(ptrbuf,day);
        ptrbuf[strlen(day)]='\0';
        ptrbuf+=strlen(day)+1;

        return buf;
        }else{
            cout<<"QRAC"<<endl;
        }



    }else{

    }
}

int GDate::int2char(int i, char **res)
{
    int bufferSize=0;
    int tmp=i;
    while(tmp!=0){
        bufferSize++;
        tmp/=10;
    }
    if(bufferSize<2){
        bufferSize=2;
    }
    char *ptr=new char[bufferSize+1];
    ptr[0]='0';
    *res=ptr;

    for(int j=0;j<bufferSize;j++){
        (*res)[(bufferSize-1)-j]=(i%10)|48;
        i/=10;
    }
    (*res)[bufferSize]='\0';
    return bufferSize;

}

void GDate::clearTm(tm *mTm)
{
    mTm->tm_sec=0;
    mTm->tm_min=0;
    mTm->tm_hour=0;
}
