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
#include <evnetmessagebuilder.h>

using namespace std;

class FileProvider
{
public:
    FileProvider();
    InternalMessage *getRadnikPicture(InternalMessage *r);
    InternalMessage *getRadnikActionPicture(InternalMessage *r);
    bool saveRadnikPicture(InternalMessage *r);
    bool saveRadnikActionPicture(InternalMessage *r);
private:
    std::string picLoc;
};

#endif // FILEPROVIDER_H
