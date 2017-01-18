#ifndef ABSTRACTDELEGATE_H
#define ABSTRACTDELEGATE_H


class AbstractDelegate
{
public:
    AbstractDelegate();
    virtual bool proccess(char *msg,int msglen)=0;
    virtual int encrypt(char *msg,int msglen)=0;
    virtual int decrypt(char *msg,int msglen)=0;
};

#endif // ABSTRACTDELEGATE_H
