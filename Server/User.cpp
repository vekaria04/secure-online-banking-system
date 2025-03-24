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
