#include "evprotocolmsg.h"

EvProtocolMsg::EvProtocolMsg()
{

}

EvProtocolHeader *EvProtocolMsg::getHeader()
{
    return &header;
}

EvProtocolPayload *EvProtocolMsg::getPayload()
{
    return *payload;
}

void EvProtocolMsg::setHeader(EvProtocolHeader &evProtocolHeader)
{

}

void EvProtocolMsg::setPayload(EvProtocolPayload &evProtocolPayload)
{

}
