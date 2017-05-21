#ifndef FILEPROVIDER_H
#define FILEPROVIDER_H
#include "internalmessage.h"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include "messageparser.h"
#include <QFile>
#include <QDir>
#include <QDateTime>

using namespace std;

class FileProvider
{
public:
    FileProvider();
    InternalMessage *getRadnikPicture(MessageParser *r);
    InternalMessage *getRadnikActionPicture(MessageParser *r);
    bool saveRadnikPicture(MessageParser *r);
    bool saveRadnikActionPicture(MessageParser *r);
private:
    std::string picLoc;
};

#endif // FILEPROVIDER_H
