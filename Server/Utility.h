onal>
using namespace std;


inline time_t getCurrentTime() {
   return time(nullptr);
}


inline string hashPassword(const string &password) {
   size_t hashVal = hash<string>{}(password);
   return to_string(hashVal);
}


#endif // UTILITY_H
