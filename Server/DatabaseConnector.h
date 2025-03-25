#ifndef DATABASECONNECTOR_H
#define DATABASECONNECTOR_H


#include <string>
#include <vector>
#include <mutex>
#include <sqlite3.h>
#include "Utility.h"


using namespace std;


// Data structures for users, accounts, and transactions.
struct UserData {
   int user_id;
   string name;
   string email;
   string password_hash;
   time_t created_at;
};


struct AccountData {
   int account_id;
   int user_id;
   string account_type;
   double balance;
   time_t created_at;
};


struct TransactionData {
   int transaction_id;
   int account_id;
   string transaction_type;
   double amount;
   time_t timestamp;
};


class DatabaseConnector {
private:
   sqlite3 *db;
   mutex db_mutex;
   mutex txn_mutex;
   bool executeSQL(const string &sql);
public:
   DatabaseConnector(const string &dbFile = "banking.db");
   ~DatabaseConnector();


   bool userExistsByEmail(const string &email);
   int insertUser(const string &name, const string &email, const string &password_hash);
   UserData* fetchUserByEmail(const string &email);
   UserData* fetchUserById(int user_id);
   bool updateUserPassword(int user_id, const string &newPasswordHash);


   int insertAccount(int user_id, const string &account_type, double initialBalance);
   AccountData* fetchAccount(int account_id);
   bool updateAccountBalance(int account_id, double newBalance);


   int insertTransaction(int account_id, const string &transaction_type, double amount, time_t timestamp);
   TransactionData* fetchTransaction(int transaction_id);
   vector<TransactionData> getTransactionsForAccount(int account_id);


   // Helper function: Returns the SQLite database handle.
   sqlite3* getDB() { return db; }
};


#endif // DATABASECONNECTOR_H
