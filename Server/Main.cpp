#include "httplib.h"
#include "DatabaseConnector.h"
#include "User.h"
#include "Account.h"
#include "TransactionManager.h"
#include "APIController.h"
#include "UserManager.h"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;
using namespace httplib;

int getAccountIdByUserId(DatabaseConnector &db, int user_id) {
    sqlite3* dbHandle = db.getDB();
    int account_id = -1;
    std::string sql = "SELECT account_id FROM Accounts WHERE user_id = ? LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(dbHandle, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, user_id);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            account_id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    return account_id;
}

int main() {
    Server svr;

    DatabaseConnector db("banking.db");
    User user(&db);
    Account account(&db);
    TransactionManager txnManager(&db, &account);
    APIController api(&db, &user, &account, &txnManager);
    UserManager userManager(&db, &user);

    svr.Post("/register", [&](const Request& req, Response& res) {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }

        std::string name = body["name"], email = body["email"], password = body["password"];
        if (!userManager.createUser(name, email, password)) {
            res.status = 400;
            res.set_content("Email already exists", "text/plain");
            return;
        }

        UserData* newUser = db.fetchUserByEmail(email);
        int userId = newUser->user_id;
        delete newUser;

        int accountId = account.createAccount(userId, "Checking");
        json response = { {"userId", userId}, {"accountId", accountId} };
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/login", [&](const Request& req, Response& res) {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }

        std::string email = body["email"], password = body["password"];
        if (!api.handleLogin(email, password)) {
            res.status = 401;
            res.set_content("Login failed", "text/plain");
            return;
        }

        UserData* loggedUser = db.fetchUserByEmail(email);
        int userId = loggedUser->user_id;
        int accountId = getAccountIdByUserId(db, userId);
        delete loggedUser;

        json response = { {"userId", userId}, {"accountId", accountId} };
        res.set_content(response.dump(), "application/json");
    });

    svr.Get(R"(/balance/(\d+))", [&](const Request& req, Response& res) {
        int accountId = std::stoi(req.matches[1]);
        double bal = api.getAccountBalance(accountId);
        json response = { {"balance", bal} };
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/transaction", [&](const Request& req, Response& res) {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }

        std::string type = body["type"];
        int accountId = body["accountId"];
        double amount = body["amount"];
        int receiverId = body.contains("receiverAccountId") ? body["receiverAccountId"].get<int>() : -1;


        bool success = api.handleTransaction(type, accountId, amount, receiverId);
        if (success) res.status = 200;
        else {
            res.status = 400;
            res.set_content("Transaction failed", "text/plain");
        }
    });

    svr.Get(R"(/transactions/(\d+))", [&](const Request& req, Response& res) {
        int accountId = std::stoi(req.matches[1]);
        std::vector<TransactionData> txns = api.getTransactionHistory(accountId);

        json response = json::array();
        for (const auto& txn : txns) {
            response.push_back({
                {"transaction_id", txn.transaction_id},
                {"transaction_type", txn.transaction_type},
                {"amount", txn.amount},
                {"timestamp", txn.timestamp}
            });
        }
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/update-password", [&](const Request& req, Response& res) {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }

        int userId = body["userId"];
        std::string oldPass = body["oldPass"], newPass = body["newPass"];
        bool success = user.updatePassword(userId, oldPass, newPass);
        if (success) res.status = 200;
        else {
            res.status = 400;
            res.set_content("Password update failed", "text/plain");
        }
    });

    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("C++ Banking API Running", "text/plain");
    });

    std::cout << "Server is running on http://localhost:3001" << std::endl;
    svr.listen("0.0.0.0", 3001);
    return 0;
}
