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


void MessageAsyncProccessor::addMessage(MessageParser *p)
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
        MessageParser *p=messageQueue.front();
        messageQueue.erase(messageQueue.begin());
        MessageType t=p->getMessageType();
        InternalMessage *msg=NULL;
        switch (t) {
        case MessageType::RequestStatus:
            msg=db->getResult(p);
            m_queue->addMessage(msg);
            m2.unlock();
            break;
        case MessageType::RequestRadnikPicture:
            msg=file->getRadnikPicture(p);
            cout<<"SALJEM ZAHTJEV ZA SLIKU bajti"<<msg->getDataSize()<<endl;
            m_queue->addMessage(msg);
            m2.unlock();
            break;
        case MessageType::Ulaz:
        case MessageType::Izlaz:
        case MessageType::TerenPocetak:
        case MessageType::TerenKraj:
        case MessageType::PauzaPocetak:
        case MessageType::PauzaKraj:
        case MessageType::PrivatnoPocetak:
        case MessageType::PrivatnoKraj:
            msg=db->getResult(p);
            if(msg!=NULL){
                m_queue->addMessage(msg);
            }
            m2.unlock();
            break;
        case MessageType::UpdateSlika:
            file->saveRadnikPicture(p);
            m2.unlock();
            break;
        case MessageType::ActionSlika:
            file->saveRadnikActionPicture(p);
            m2.unlock();
            break;
        case MessageType::RequestActionSlika:
            msg=file->getRadnikActionPicture(p);
            cout<<"SALJEM ZAHTJEV ZA SLIKU bajti"<<msg->getDataSize()<<endl;
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
