#ifndef MESSAGEASYNCPROCCESSOR_H
#define MESSAGEASYNCPROCCESSOR_H
#include <messageparser.h>
#include <internalmessage.h>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <sys/msg.h>
#include <iostream>
#include <database.h>
#include <mysqldatabase.h>
#include "fileprovider.h"
#include "messagequeue.h"

using namespace std;

class MessageAsyncProccessor
{
public:
    MessageAsyncProccessor(MessageQueue *queue);
    ~MessageAsyncProccessor();
    void addMessage(InternalMessage* p);
private:
    vector<InternalMessage*> messageQueue;
    //thread stuff
    condition_variable cv;
    void run();
    thread *t1;
    mutex m1;//condition variable
    mutex m2;// atomic messageQueue;
        //thread condition
    bool threadRun;
    bool messageArrive;

    //database stuf
    Database *db;
    FileProvider *file;
    MessageQueue *m_queue;
};

#endif // MESSAGEASYNCPROCCESSOR_H
