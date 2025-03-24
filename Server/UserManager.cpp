#include "UserManager.h"


UserManager::UserManager(DatabaseConnector* connector, User* u) : db(connector), user(u) {}


bool UserManager::createUser(const string &name, const string &email, const string &password) {
   return user->registerUser(name, email, password);
}


bool UserManager::authenticateUser(const string &email, const string &password) {
   return user->authenticate(email, password);
}
