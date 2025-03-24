#include "Account.h"

Account::Account(DatabaseConnector* connector) : db(connector) {}

int Account::createAccount(int userId, const string &accountType) {
   return db->insertAccount(userId, accountType, 0.0);
}

double Account::getBalance(int accountId) {
   AccountData* account = db->fetchAccount(accountId);
   if (!account)
       return -1;
   double balance = account->balance;
   delete account;
   return balance;
}

bool Account::updateBalance(int accountId, double amount) {
   AccountData* account = db->fetchAccount(accountId);
   if (!account)
       return false;
   double newBalance = account->balance + amount;
   delete account;
   return db->updateAccountBalance(accountId, newBalance);
}
