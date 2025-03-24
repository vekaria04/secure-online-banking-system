#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H


#include <string>
#include "DatabaseConnector.h"
#include "Account.h"
#include "Transaction.h"


using namespace std;


class TransactionManager {
private:
   DatabaseConnector* db;
   Account* account;
public:
   TransactionManager(DatabaseConnector* connector, Account* acc);
   bool validateTransaction(const string &type, int accountId, double amount);
   bool executeTransaction(const string &type, int accountId, double amount, int receiverAccountId = -1);
   bool rollbackTransaction(int transactionId);
};


#endif // TRANSACTIONMANAGER_H


