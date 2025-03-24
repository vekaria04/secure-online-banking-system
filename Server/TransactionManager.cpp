#include "TransactionManager.h"


TransactionManager::TransactionManager(DatabaseConnector* connector, Account* acc) : db(connector), account(acc) {}


bool TransactionManager::validateTransaction(const string &type, int accountId, double amount) {
   if (amount <= 0)
       return false;
   if ((type == "Withdrawal" || type == "Transfer") && account->getBalance(accountId) < amount)
       return false;
   return true;
}


bool TransactionManager::executeTransaction(const string &type, int accountId, double amount, int receiverAccountId) {
   Transaction txn(db, account);
   if (!validateTransaction(type, accountId, amount))
       return false;
  
   if (type == "Deposit")
       return txn.processDeposit(accountId, amount);
   else if (type == "Withdrawal")
       return txn.processWithdrawal(accountId, amount);
   else if (type == "Transfer") {
       if (receiverAccountId == -1)
           return false;
       return txn.processTransfer(accountId, receiverAccountId, amount);
   }
   return false;
}


bool TransactionManager::rollbackTransaction(int transactionId) {
   TransactionData* txnData = db->fetchTransaction(transactionId);
   if (!txnData)
       return false;
   double reverseAmount = -txnData->amount;
   bool success = account->updateBalance(txnData->account_id, reverseAmount);
   if (success)
       db->insertTransaction(txnData->account_id, "Rollback", reverseAmount, getCurrentTime());
   delete txnData;
   return success;
}
