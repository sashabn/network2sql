#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <vector>
#include <mutex>
#include "internalmessage.h"
#include <condition_variable>


class MessageQueue
{
public:
    MessageQueue();
    void addMessage(InternalMessage *m);
    InternalMessage *getNextMessage();



private:
    std::vector<InternalMessage*> *messageQueue;
    std::condition_variable cv;
    std::mutex m1;
    std::mutex m2;
    bool messageReady;
};

#endif // MESSAGEQUEUE_H
