#ifndef USERMANAGER_H
#define USERMANAGER_H


#include <string>
#include "DatabaseConnector.h"
#include "User.h"


using namespace std;


class UserManager {
private:
   DatabaseConnector* db;
   User* user;
public:
   UserManager(DatabaseConnector* connector, User* u);
   bool createUser(const string &name, const string &email, const string &password);
   bool authenticateUser(const string &email, const string &password);
};


#endif // USERMANAGER_H