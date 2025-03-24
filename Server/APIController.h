#ifndef APICONTROLLER_H
#define APICONTROLLER_H


#include <string>
#include <vector>
#include "DatabaseConnector.h"
#include "User.h"
#include "Account.h"
#include "TransactionManager.h"


using namespace std;


class APIController {
private:
   DatabaseConnector* db;
   User* user;
   Account* account;
   TransactionManager* txnManager;
public:
   APIController(DatabaseConnector* connector, User* u, Account* a, TransactionManager* tm);
   bool handleLogin(const string &email, const string &password);
   bool handleTransaction(const string &type, int accountId, double amount, int receiverAccountId = -1);
   double getAccountBalance(int accountId);
   vector<TransactionData> getTransactionHistory(int accountId);
};


#endif // APICONTROLLER_H


