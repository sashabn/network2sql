#ifndef GDATE_H
#define GDATE_H
#include <time.h>
#include <ctime>
#include <iostream>
#include <sstream>


#include <iomanip>
#include <locale>
#include <cstring>
using namespace std;

class GDate
{
public:
    GDate();
    GDate(time_t epoch);
    static GDate *fromString(const char *date,const char *format);
    const char *toString(const char *format=NULL);
private:
    tm *my_tm;
    time_t epochTime;

protected:
    int int2char(int i,char **res);
    void clearTm(tm *mTm);
    void fillBuffer();


};

#endif // GDATE_H
