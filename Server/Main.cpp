#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include "DatabaseConnector.h"
#include "User.h"
#include "Account.h"
#include "TransactionManager.h"
#include "APIController.h"
#include "UserManager.h"


using namespace std;


// Helper function: Retrieves the account ID for a given user.
// Assumes DatabaseConnector has a public method getDB() that returns sqlite3*.
int getAccountIdByUserId(DatabaseConnector &dbc, int user_id) {
   sqlite3* dbHandle = dbc.getDB(); // Make sure this getter exists in DatabaseConnector.
   int account_id = -1;
   string sql = "SELECT account_id FROM Accounts WHERE user_id = ? LIMIT 1;";
   sqlite3_stmt* stmt = nullptr;
   if (sqlite3_prepare_v2(dbHandle, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
       sqlite3_bind_int(stmt, 1, user_id);
       if (sqlite3_step(stmt) == SQLITE_ROW) {
           account_id = sqlite3_column_int(stmt, 0);
       }
       sqlite3_finalize(stmt);
   } else {
       cerr << "Failed to prepare statement in getAccountIdByUserId." << endl;
   }
   return account_id;
}


int main() {
   // Instantiate the core components.
   DatabaseConnector db("banking.db");
   User user(&db);
   Account account(&db);
   TransactionManager txnManager(&db, &account);
   APIController api(&db, &user, &account, &txnManager);
   UserManager userManager(&db, &user);


   int mainChoice = 0;
   while (true) {
       // Main menu
       cout << "\n=== Welcome to Online Banking System ===\n";
       cout << "1. Register\n";
       cout << "2. Login\n";
       cout << "3. Exit\n";
       cout << "Enter option: ";
       cin >> mainChoice;
       cin.ignore();  // clear newline


       if (mainChoice == 3) {
           cout << "Exiting the system. Goodbye.\n";
           break;
       }


       if (mainChoice == 1) {
           // Registration process
           string name, email, password;
           cout << "\n--- Registration ---\n";
           cout << "Enter full name: ";
           getline(cin, name);
           cout << "Enter email: ";
           getline(cin, email);
           cout << "Enter password: ";
           getline(cin, password);


           if (userManager.createUser(name, email, password)) {
               cout << "Registration successful.\n";
               // Fetch the new user data to obtain the user ID.
               UserData* newUser = db.fetchUserByEmail(email);
               if (newUser) {
                   int user_id = newUser->user_id;
                   delete newUser;
                   // Create a default account ("Checking") for the new user.
                   int acct_id = account.createAccount(user_id, "Checking");
                   if (acct_id != -1)
                       cout << "Account created. Your account ID is: " << acct_id << "\n";
                   else
                       cout << "Account creation failed.\n";
               }
           } else {
               cout << "Registration failed. Email may already be in use.\n";
           }
       }
       else if (mainChoice == 2) {
           // Login process
           string email, password;
           cout << "\n--- Login ---\n";
           cout << "Enter email: ";
           getline(cin, email);
           cout << "Enter password: ";
           getline(cin, password);


           if (api.handleLogin(email, password)) {
               cout << "Login successful.\n";
               // Retrieve logged-in user's data.
               UserData* loggedUser = db.fetchUserByEmail(email);
               if (!loggedUser) {
                   cout << "Error retrieving user data. Returning to main menu.\n";
                   continue;
               }
               int user_id = loggedUser->user_id;
               delete loggedUser;


               // Retrieve the account ID for the logged-in user.
               int acct_id = getAccountIdByUserId(db, user_id);
               if (acct_id == -1) {
                   cout << "No account found for user. Creating a default account.\n";
                   acct_id = account.createAccount(user_id, "Checking");
               }


               // Logged-in user menu.
               bool loggedIn = true;
               while (loggedIn) {
                   cout << "\n--- Account Menu ---\n";
                   cout << "1. Deposit\n";
                   cout << "2. Withdraw\n";
                   cout << "3. Transfer\n";
                   cout << "4. Check Balance\n";
                   cout << "5. Transaction History\n";
                   cout << "6. Update Password\n";
                   cout << "7. Logout\n";
                   cout << "Enter option: ";
                   int accChoice = 0;
                   cin >> accChoice;
                   cin.ignore();


                   switch (accChoice) {
                       case 1: { // Deposit
                           double amount = 0;
                           cout << "Enter deposit amount: ";
                           cin >> amount;
                           cin.ignore();
                           if (api.handleTransaction("Deposit", acct_id, amount))
                               cout << "Deposit successful.\n";
                           else
                               cout << "Deposit failed.\n";
                           break;
                       }
                       case 2: { // Withdraw
                           double amount = 0;
                           cout << "Enter withdrawal amount: ";
                           cin >> amount;
                           cin.ignore();
                           if (api.handleTransaction("Withdrawal", acct_id, amount))
                               cout << "Withdrawal successful.\n";
                           else
                               cout << "Withdrawal failed (check available balance).\n";
                           break;
                       }
                       case 3: { // Transfer
                           double amount = 0;
                           string recipientEmail;
                           cout << "Enter transfer amount: ";
                           cin >> amount;
                           cin.ignore();
                           cout << "Enter recipient's email: ";
                           getline(cin, recipientEmail);
                           // Look up the recipient's account.
                           UserData* recipient = db.fetchUserByEmail(recipientEmail);
                           if (!recipient) {
                               cout << "Recipient not found.\n";
                           } else {
                               int recipient_id = recipient->user_id;
                               delete recipient;
                               int recipient_acct = getAccountIdByUserId(db, recipient_id);
                               if (recipient_acct == -1)
                                   cout << "Recipient does not have an account.\n";
                               else {
                                   if (api.handleTransaction("Transfer", acct_id, amount, recipient_acct))
                                       cout << "Transfer successful.\n";
                                   else
                                       cout << "Transfer failed (check balance and amount).\n";
                               }
                           }
                           break;
                       }
                       case 4: { // Check Balance
                           double bal = api.getAccountBalance(acct_id);
                           cout << "Your balance: $" << bal << "\n";
                           break;
                       }
                       case 5: { // Transaction History
                           vector<TransactionData> history = api.getTransactionHistory(acct_id);
                           cout << "\n--- Transaction History ---\n";
                           if (history.empty())
                               cout << "No transactions found.\n";
                           else {
                               for (const auto &txn : history) {
                                   cout << "ID: " << txn.transaction_id
                                        << ", Type: " << txn.transaction_type
                                        << ", Amount: $" << txn.amount
                                        << ", Timestamp: " << txn.timestamp << "\n";
                               }
                           }
                           break;
                       }
                       case 6: { // Update Password
                           string oldPass, newPass;
                           cout << "Enter old password: ";
                           getline(cin, oldPass);
                           cout << "Enter new password: ";
                           getline(cin, newPass);
                           if (user.updatePassword(user_id, oldPass, newPass))
                               cout << "Password updated successfully.\n";
                           else
                               cout << "Password update failed. (Check your old password)\n";
                           break;
                       }
                       case 7: {
                           loggedIn = false;
                           cout << "Logging out...\n";
                           break;
                       }
                       default:
                           cout << "Invalid option. Please try again.\n";
                   }
               }
           } else {
               cout << "Login failed. Please check your credentials.\n";
           }
       }
   }
   return 0;
}
