#include "User.h"

User::User(DatabaseConnector* connector) : db(connector) {}

bool User::registerUser(const string &name, const string &email, const string &password) {
   string hashedPassword = hashPassword(password);
   int userId = db->insertUser(name, email, hashedPassword);
   return userId != -1;
}


bool User::authenticate(const string &email, const string &password) {
   UserData* user = db->fetchUserByEmail(email);
   if (!user)
       return false;
   string hashedPassword = hashPassword(password);
   bool valid = (hashedPassword == user->password_hash);
   delete user;
   return valid;
}


bool User::updatePassword(int user_id, const string &oldPassword, const string &newPassword) {
   UserData* user = db->fetchUserById(user_id);
   if (!user)
       return false;
   if (hashPassword(oldPassword) == user->password_hash) {
       bool result = db->updateUserPassword(user_id, hashPassword(newPassword));
       delete user;
       return result;
   }
   delete user;
   return false;
}
bool User::updateProfile(const string &name, const string &email, int userId) {
    sqlite3* dbHandle = db->getDB();
    string sql = "UPDATE Users SET name = ?, email = ? WHERE user_id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(dbHandle, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, userId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
