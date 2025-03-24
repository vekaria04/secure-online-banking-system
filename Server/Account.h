#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include "DatabaseConnector.h"

using namespace std;

class Account {
private:
   DatabaseConnector* db;
public:
   Account(DatabaseConnector* connector);
   int createAccount(int userId, const string &accountType);
   double getBalance(int accountId);
   bool updateBalance(int accountId, double amount);
};


#endif // ACCOUNT_H
