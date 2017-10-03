#include "messageasyncproccessor.h"

MessageAsyncProccessor::MessageAsyncProccessor(MessageQueue *mId):m_queue(mId)
{
    threadRun=true;
    t1=NULL;
    db=new MysqlDatabase("127.0.0.1","root","l0lc1na","evidencija");
    file=new FileProvider;
    t1=new thread(&MessageAsyncProccessor::run,this);
}

MessageAsyncProccessor::~MessageAsyncProccessor()
{
    threadRun=false;
    if(t1!=NULL)
        if(t1->joinable())
            t1->join();
}


void MessageAsyncProccessor::addMessage(InternalMessage *p)
{
    if(p==NULL){
        return;
    }
    m2.lock();
    messageQueue.push_back(p);
    m2.unlock();
    messageArrive=true;
    cv.notify_all();

}

void MessageAsyncProccessor::run()
{
    for(;threadRun;){
        unique_lock<mutex> lk(m1);
        cv.wait(lk,[&]{
            return messageArrive;
        });

        m2.lock();
        if(messageQueue.size()==0){
            messageArrive=false;
            m2.unlock();
            continue;
        }
        InternalMessage *p=messageQueue.front();
        messageQueue.erase(messageQueue.begin());
        NetworkAPI t=p->getMsg()->getHdr().getApiId();
        InternalMessage *msg=NULL;
        switch (t) {
        case NetworkAPI::RequestStatus:
            cout<<"Request status "<<endl;
            msg=db->getResult(p);
            m_queue->addMessage(msg);
            m2.unlock();
            break;
        case NetworkAPI::RequestEmployeePicture:
            msg=file->getRadnikPicture(p);
            cout<<"Sending request for picture with size: "<<msg->getMsg()->getSize()<<endl;
            m_queue->addMessage(msg);
            m2.unlock();
            break;
        case NetworkAPI::JobStart:
        case NetworkAPI::JobEnd:
        case NetworkAPI::WorkOutStart:
        case NetworkAPI::WorkOutEnd:
        case NetworkAPI::PauseStart:
        case NetworkAPI::PauseEnd:
        case NetworkAPI::PrivateStart:
        case NetworkAPI::PrivateEnd:
            msg=db->getResult(p);
            m2.unlock();
            break;
        case NetworkAPI::UpdateEmployeeProfilePicture:
            file->saveRadnikPicture(p);
            m2.unlock();
            break;
        case NetworkAPI::TimePicture:
            file->saveRadnikActionPicture(p);
            m2.unlock();
            break;
        case NetworkAPI::RequestTimePicture:
            msg=file->getRadnikActionPicture(p);
            m_queue->addMessage(msg);
            m2.unlock();
            break;

        default:
            m2.unlock();
            break;
        }
        delete p;
    }
}
