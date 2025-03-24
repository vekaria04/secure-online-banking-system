#include "APIController.h"

APIController::APIController(DatabaseConnector* connector, User* u, Account* a, TransactionManager* tm)
   : db(connector), user(u), account(a), txnManager(tm) {}


bool APIController::handleLogin(const string &email, const string &password) {
   return user->authenticate(email, password);
}


bool APIController::handleTransaction(const string &type, int accountId, double amount, int receiverAccountId) {
   return txnManager->executeTransaction(type, accountId, amount, receiverAccountId);
}


double APIController::getAccountBalance(int accountId) {
   return account->getBalance(accountId);
}


vector<TransactionData> APIController::getTransactionHistory(int accountId) {
   return db->getTransactionsForAccount(accountId);
}
