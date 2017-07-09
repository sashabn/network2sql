#ifndef EVPROTOCOLPAYLOAD_H
#define EVPROTOCOLPAYLOAD_H


class EvProtocolPayload
{
public:
    EvProtocolPayload();
    virtual int getByteCount()=0;
};

#endif // EVPROTOCOLPAYLOAD_H
