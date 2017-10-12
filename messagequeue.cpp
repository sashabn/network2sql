#include "messagequeue.h"

MessageQueue::MessageQueue()
{
    messageQueue=new std::vector<InternalMessage*>;
    messageReady=false;
}

void MessageQueue::addMessage(InternalMessage *m)
{
       m2.lock();
       cout<<"Adding message to queue"<<endl;
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
    if(nextMessage!=NULL){
        cout<<"Success taking message from queue  api: "<<nextMessage->getMsg()->getHdr().getApiId()<<" Socket fd "<<nextMessage->getFd()<<endl;
    }else{
        cout<<"Message from queue is null "<<endl;
    }
    messageQueue->erase(messageQueue->begin());
    if(messageQueue->size()==0){
        messageReady=false;
    }
    m2.unlock();
    return nextMessage;
}
