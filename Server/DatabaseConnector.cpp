#include "DatabaseConnector.h"
#include <iostream>


using namespace std;


bool DatabaseConnector::executeSQL(const string &sql) {
   char *errMsg = nullptr;
   lock_guard<mutex> lock(db_mutex);
   int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
   if (rc != SQLITE_OK) {
       cerr << "SQL error: " << errMsg << endl;
       sqlite3_free(errMsg);
       return false;
   }
   return true;
}


DatabaseConnector::DatabaseConnector(const string &dbFile) {
   int rc = sqlite3_open(dbFile.c_str(), &db);
   if (rc) {
       cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
       db = nullptr;
   } else {
       // Create tables if they do not exist.
       string createUsers = "CREATE TABLE IF NOT EXISTS Users ("
                            "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "name TEXT NOT NULL, "
                            "email TEXT UNIQUE NOT NULL, "
                            "password_hash TEXT NOT NULL, "
                            "created_at INTEGER NOT NULL);";
       string createAccounts = "CREATE TABLE IF NOT EXISTS Accounts ("
                               "account_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "user_id INTEGER NOT NULL, "
                               "account_type TEXT NOT NULL, "
                               "balance REAL NOT NULL, "
                               "created_at INTEGER NOT NULL, "
                               "FOREIGN KEY(user_id) REFERENCES Users(user_id));";
       string createTransactions = "CREATE TABLE IF NOT EXISTS Transactions ("
                                   "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "account_id INTEGER NOT NULL, "
                                   "transaction_type TEXT NOT NULL, "
                                   "amount REAL NOT NULL, "
                                   "timestamp INTEGER NOT NULL, "
                                   "FOREIGN KEY(account_id) REFERENCES Accounts(account_id));";
       executeSQL(createUsers);
       executeSQL(createAccounts);
       executeSQL(createTransactions);
   }
}


DatabaseConnector::~DatabaseConnector() {
   if (db) sqlite3_close(db);
}


bool DatabaseConnector::userExistsByEmail(const string &email) {
   lock_guard<mutex> lock(db_mutex);
   string sql = "SELECT COUNT(*) FROM Users WHERE email = ?;";
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if (rc != SQLITE_OK) {
       cerr << "Prepare statement failed." << endl;
       return false;
   }
   sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
   rc = sqlite3_step(stmt);
   bool exists = false;
   if (rc == SQLITE_ROW) {
       int count = sqlite3_column_int(stmt, 0);
       exists = (count > 0);
   }
   sqlite3_finalize(stmt);
   return exists;
}


int DatabaseConnector::insertUser(const string &name, const string &email, const string &password_hash) {
   if(userExistsByEmail(email)) return -1;
   string sql = "INSERT INTO Users (name, email, password_hash, created_at) VALUES (?, ?, ?, ?);";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if (rc != SQLITE_OK) {
       cerr << "Prepare insert user failed." << endl;
       return -1;
   }
   sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_text(stmt, 3, password_hash.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_int(stmt, 4, (int)getCurrentTime());
   rc = sqlite3_step(stmt);
   if (rc != SQLITE_DONE) {
       cerr << "Insert user execution failed." << endl;
       sqlite3_finalize(stmt);
       return -1;
   }
   sqlite3_finalize(stmt);
   return (int)sqlite3_last_insert_rowid(db);
}


UserData* DatabaseConnector::fetchUserByEmail(const string &email) {
   string sql = "SELECT user_id, name, email, password_hash, created_at FROM Users WHERE email = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if (rc != SQLITE_OK) {
       cerr << "Prepare fetch user failed." << endl;
       return nullptr;
   }
   sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
   UserData* user = nullptr;
   if (sqlite3_step(stmt) == SQLITE_ROW) {
       user = new UserData;
       user->user_id = sqlite3_column_int(stmt, 0);
       user->name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
       user->email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
       user->password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
       user->created_at = sqlite3_column_int(stmt, 4);
   }
   sqlite3_finalize(stmt);
   return user;
}


UserData* DatabaseConnector::fetchUserById(int user_id) {
   string sql = "SELECT user_id, name, email, password_hash, created_at FROM Users WHERE user_id = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if (rc != SQLITE_OK) {
       cerr << "Prepare fetch user by ID failed." << endl;
       return nullptr;
   }
   sqlite3_bind_int(stmt, 1, user_id);
   UserData* user = nullptr;
   if (sqlite3_step(stmt) == SQLITE_ROW) {
       user = new UserData;
       user->user_id = sqlite3_column_int(stmt, 0);
       user->name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
       user->email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
       user->password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
       user->created_at = sqlite3_column_int(stmt, 4);
   }
   sqlite3_finalize(stmt);
   return user;
}


bool DatabaseConnector::updateUserPassword(int user_id, const string &newPasswordHash) {
   string sql = "UPDATE Users SET password_hash = ? WHERE user_id = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare update password failed." << endl;
       return false;
   }
   sqlite3_bind_text(stmt, 1, newPasswordHash.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_int(stmt, 2, user_id);
   rc = sqlite3_step(stmt);
   sqlite3_finalize(stmt);
   return rc == SQLITE_DONE;
}


int DatabaseConnector::insertAccount(int user_id, const string &account_type, double initialBalance) {
   string sql = "INSERT INTO Accounts (user_id, account_type, balance, created_at) VALUES (?, ?, ?, ?);";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare insert account failed." << endl;
       return -1;
   }
   sqlite3_bind_int(stmt, 1, user_id);
   sqlite3_bind_text(stmt, 2, account_type.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_double(stmt, 3, initialBalance);
   sqlite3_bind_int(stmt, 4, (int)getCurrentTime());
   rc = sqlite3_step(stmt);
   if(rc != SQLITE_DONE) {
       cerr << "Insert account execution failed." << endl;
       sqlite3_finalize(stmt);
       return -1;
   }
   sqlite3_finalize(stmt);
   return (int)sqlite3_last_insert_rowid(db);
}


AccountData* DatabaseConnector::fetchAccount(int account_id) {
   string sql = "SELECT account_id, user_id, account_type, balance, created_at FROM Accounts WHERE account_id = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare fetch account failed." << endl;
       return nullptr;
   }
   sqlite3_bind_int(stmt, 1, account_id);
   AccountData* account = nullptr;
   if(sqlite3_step(stmt) == SQLITE_ROW) {
       account = new AccountData;
       account->account_id = sqlite3_column_int(stmt, 0);
       account->user_id = sqlite3_column_int(stmt, 1);
       account->account_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
       account->balance = sqlite3_column_double(stmt, 3);
       account->created_at = sqlite3_column_int(stmt, 4);
   }
   sqlite3_finalize(stmt);
   return account;
}


bool DatabaseConnector::updateAccountBalance(int account_id, double newBalance) {
   string sql = "UPDATE Accounts SET balance = ? WHERE account_id = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare update account balance failed." << endl;
       return false;
   }
   sqlite3_bind_double(stmt, 1, newBalance);
   sqlite3_bind_int(stmt, 2, account_id);
   rc = sqlite3_step(stmt);
   sqlite3_finalize(stmt);
   return rc == SQLITE_DONE;
}


int DatabaseConnector::insertTransaction(int account_id, const string &transaction_type, double amount, time_t timestamp) {
   string sql = "INSERT INTO Transactions (account_id, transaction_type, amount, timestamp) VALUES (?, ?, ?, ?);";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare insert transaction failed." << endl;
       return -1;
   }
   sqlite3_bind_int(stmt, 1, account_id);
   sqlite3_bind_text(stmt, 2, transaction_type.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_double(stmt, 3, amount);
   sqlite3_bind_int(stmt, 4, (int)timestamp);
   rc = sqlite3_step(stmt);
   if(rc != SQLITE_DONE) {
       cerr << "Insert transaction execution failed." << endl;
       sqlite3_finalize(stmt);
       return -1;
   }
   sqlite3_finalize(stmt);
   return (int)sqlite3_last_insert_rowid(db);
}


TransactionData* DatabaseConnector::fetchTransaction(int transaction_id) {
   string sql = "SELECT transaction_id, account_id, transaction_type, amount, timestamp FROM Transactions WHERE transaction_id = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare fetch transaction failed." << endl;
       return nullptr;
   }
   sqlite3_bind_int(stmt, 1, transaction_id);
   TransactionData* txn = nullptr;
   if(sqlite3_step(stmt) == SQLITE_ROW) {
       txn = new TransactionData;
       txn->transaction_id = sqlite3_column_int(stmt, 0);
       txn->account_id = sqlite3_column_int(stmt, 1);
       txn->transaction_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
       txn->amount = sqlite3_column_double(stmt, 3);
       txn->timestamp = sqlite3_column_int(stmt, 4);
   }
   sqlite3_finalize(stmt);
   return txn;
}


vector<TransactionData> DatabaseConnector::getTransactionsForAccount(int account_id) {
   vector<TransactionData> history;
   string sql = "SELECT transaction_id, account_id, transaction_type, amount, timestamp FROM Transactions WHERE account_id = ?;";
   lock_guard<mutex> lock(db_mutex);
   sqlite3_stmt *stmt;
   int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
   if(rc != SQLITE_OK) {
       cerr << "Prepare get transactions failed." << endl;
       return history;
   }
   sqlite3_bind_int(stmt, 1, account_id);
   while(sqlite3_step(stmt) == SQLITE_ROW) {
       TransactionData txn;
       txn.transaction_id = sqlite3_column_int(stmt, 0);
       txn.account_id = sqlite3_column_int(stmt, 1);
       txn.transaction_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
       txn.amount = sqlite3_column_double(stmt, 3);
       txn.timestamp = sqlite3_column_int(stmt, 4);
       history.push_back(txn);
   }
   sqlite3_finalize(stmt);
   return history;
}
