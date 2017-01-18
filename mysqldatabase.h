#ifndef MYSQLDATABASE_H
#define MYSQLDATABASE_H
#include "database.h"
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <string>


using namespace sql;

using namespace std;

class MysqlDatabase : public Database
{
public:
    MysqlDatabase();
private:
    Driver *dbDriver;
    Statement *dbStatment;
    Connection *dbConnection;
    ResultSet *dbResult;


};

#endif // MYSQLDATABASE_H
