#ifndef USER_H
#define USER_H


#include <string>
#include "DatabaseConnector.h"
#include "Utility.h"


using namespace std;


class User {
private:
   DatabaseConnector* db;
public:
   User(DatabaseConnector* connector);
   bool registerUser(const string &name, const string &email, const string &password);
   bool authenticate(const string &email, const string &password);
   bool updatePassword(int user_id, const string &oldPassword, const string &newPassword);
};


#endif // USER_H