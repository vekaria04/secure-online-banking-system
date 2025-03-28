g++ -std=c++17 -I./third_party/jwt-cpp/include Main.cpp Account.cpp APIController.cpp Transaction.cpp TransactionManager.cpp User.cpp UserManager.cpp -o seead -lws2_32 -lssl -lcrypto



Use this on WSL 
g++ -std=c++17 -I./third_party/jwt-cpp/include Main.cpp Account.cpp APIController.cpp Transaction.cpp TransactionManager.cpp User.cpp UserManager.cpp DatabaseConnector.cpp -o seead -lssl -lcrypto -lpthread -lsqlite3