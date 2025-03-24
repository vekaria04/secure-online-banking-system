#include "Transaction.h"


Transaction::Transaction(DatabaseConnector* connector, Account* acc) : db(connector), account(acc) {}


bool Transaction::processDeposit(int accountId, double amount) {
   if (amount <= 0)
       return false;
   bool success = account->updateBalance(accountId, amount);
   if (success)
       db->insertTransaction(accountId, "Deposit", amount, getCurrentTime());
   return success;
}


bool Transaction::processWithdrawal(int accountId, double amount) {
   if (amount <= 0)
       return false;
   double balance = account->getBalance(accountId);
   if (balance < amount)
       return false;
   bool success = account->updateBalance(accountId, -amount);
   if (success)
       db->insertTransaction(accountId, "Withdrawal", amount, getCurrentTime());
   return success;
}


bool Transaction::processTransfer(int senderAccountId, int receiverAccountId, double amount) {
   if (amount <= 0)
       return false;
   double senderBalance = account->getBalance(senderAccountId);
   if (senderBalance < amount)
       return false;
   bool senderSuccess = account->updateBalance(senderAccountId, -amount);
   bool receiverSuccess = account->updateBalance(receiverAccountId, amount);
   if (senderSuccess && receiverSuccess) {
       db->insertTransaction(senderAccountId, "Transfer Out", amount, getCurrentTime());
       db->insertTransaction(receiverAccountId, "Transfer In", amount, getCurrentTime());
       return true;
   } else {
       if (senderSuccess) account->updateBalance(senderAccountId, amount);
       if (receiverSuccess) account->updateBalance(receiverAccountId, -amount);
       return false;
   }
}


