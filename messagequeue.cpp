#include "messagequeue.h"

MessageQueue::MessageQueue()
{
    messageQueue=new std::vector<InternalMessage*>;
    messageReady=false;
}

void MessageQueue::addMessage(InternalMessage *m)
{
       m2.lock();
       messageQueue->push_back(m);
       messageReady=true;
       m2.unlock();

       cv.notify_all();
}

InternalMessage *MessageQueue::getNextMessage()
{
    InternalMessage *nextMessage=NULL;
    std::unique_lock<std::mutex> lk(m1);
    cv.wait(lk,[&]{return messageReady;});
    m2.lock();
    nextMessage=messageQueue->front();
    messageQueue->erase(messageQueue->begin());
    if(messageQueue->size()==0){
        messageReady=false;
    }
    m2.unlock();
    return nextMessage;
}
