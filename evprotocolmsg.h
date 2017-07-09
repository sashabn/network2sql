#ifndef EVPROTOCOLMSG_H
#define EVPROTOCOLMSG_H
#include "evprotocolheader.h"
#include "evprotocolpayload.h"

class EvProtocolMsg
{
public:
    EvProtocolMsg();

    EvProtocolHeader *getHeader();
    EvProtocolPayload *getPayload();

    void setHeader(EvProtocolHeader &evProtocolHeader);
    void setPayload(EvProtocolPayload &evProtocolPayload);

    int getMessageByteCnt();

private:
   EvProtocolHeader header;
   EvProtocolPayload *payload[1];

   int byteCount;
};

#endif // EVPROTOCOLMSG_H
