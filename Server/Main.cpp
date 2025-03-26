#include "httplib.h"
#include "DatabaseConnector.h"
#include "User.h"
#include "Account.h"
#include "TransactionManager.h"
#include "APIController.h"
#include "UserManager.h"
#include "json.hpp"
#include <iostream>

// Include jwt-cpp
#define JWT_DISABLE_BASE64
#define JWT_DISABLE_PICOJSON
#include "jwt-cpp/jwt.h"
#include <chrono>

using json = nlohmann::json;
using namespace httplib;

// A secret key for signing JWTs (in production, store this securely)
static const std::string JWT_SECRET = "my_super_secret_key";

// Helper function to fetch the first account ID for a user.
int getAccountIdByUserId(DatabaseConnector &db, int user_id) {
    sqlite3* dbHandle = db.getDB();
    int account_id = -1;
    std::string sql = "SELECT account_id FROM Accounts WHERE user_id = ? LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(dbHandle, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, user_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            account_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    return account_id;
}

// Validates the JWT from the "Authorization: Bearer <token>" header.
// Returns true if valid, and sets userId to the "sub" claim from the token.
bool validate_jwt(const Request &req, std::string &userId) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.rfind("Bearer ", 0) != 0) {
        std::cerr << "Missing or invalid Authorization header.\n";
        return false;
    }
    std::string token = authHeader.substr(7); // remove "Bearer "

    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
            .with_issuer("my-issuer");
        verifier.verify(decoded);

        // Extract the subject ("sub") as the user ID
        userId = decoded.get_subject();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "JWT validation failed: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    Server svr;

    // Temporary CORS solution: allow all origins
    svr.set_pre_routing_handler([](auto &req, auto &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        if (req.method == "OPTIONS") {
            res.status = 200;
            return Server::HandlerResponse::Handled;
        }
        return Server::HandlerResponse::Unhandled;
    });

    DatabaseConnector db("banking.db");
    User user(&db);
    Account account(&db);
    TransactionManager txnManager(&db, &account);
    APIController api(&db, &user, &account, &txnManager);
    UserManager userManager(&db, &user);

    // -------------------------
    //  REGISTER
    // -------------------------
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
        if (!newUser) {
            res.status = 500;
            res.set_content("Registration error: user not found after creation", "text/plain");
            return;
        }
        int userId = newUser->user_id;
        delete newUser;
        int accountId = account.createAccount(userId, "Checking");
        json response = { {"userId", userId}, {"accountId", accountId} };
        res.set_content(response.dump(), "application/json");
    });

    // -------------------------
    //  LOGIN
    // -------------------------
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
        if (!loggedUser) {
            res.status = 401;
            res.set_content("Login failed: user not found", "text/plain");
            return;
        }
        int userId = loggedUser->user_id;
        int accountId = getAccountIdByUserId(db, userId);
        delete loggedUser;

        // Generate JWT token
        auto token = jwt::create()
            .set_issuer("my-issuer")
            .set_type("JWS")
            .set_audience("my-client")
            .set_subject(std::to_string(userId))
            .set_payload_claim("accountId", jwt::claim(std::to_string(accountId)))
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(1))
            .sign(jwt::algorithm::hs256{JWT_SECRET});

        json response = { {"userId", userId}, {"accountId", accountId}, {"token", token} };
        res.set_content(response.dump(), "application/json");
    });

    // -------------------------
    //  BALANCE
    // -------------------------
    svr.Get(R"(/balance/(\d+))", [&](const Request& req, Response& res) {
        int accountId = std::stoi(req.matches[1]);
        double bal = api.getAccountBalance(accountId);
        json response = { {"balance", bal} };
        res.set_content(response.dump(), "application/json");
    });

    // -------------------------
    //  DEPOSIT (Protected)
    // -------------------------
    svr.Post("/deposit", [&](const Request& req, Response& res) {
        std::string tokenUserId;
        if (!validate_jwt(req, tokenUserId)) {
            res.status = 401;
            res.set_content("Unauthorized", "text/plain");
            return;
        }
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }
        int accountId = body["accountId"];
        double amount = body["amount"];
        bool success = api.handleTransaction("Deposit", accountId, amount, -1);
        if (success)
            res.status = 200;
        else {
            res.status = 400;
            res.set_content("Deposit failed", "text/plain");
        }
    });

    // -------------------------
    //  WITHDRAW (Protected)
    // -------------------------
    svr.Post("/withdraw", [&](const Request& req, Response& res) {
        std::string tokenUserId;
        if (!validate_jwt(req, tokenUserId)) {
            res.status = 401;
            res.set_content("Unauthorized", "text/plain");
            return;
        }
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }
        int accountId = body["accountId"];
        double amount = body["amount"];
        bool success = api.handleTransaction("Withdrawal", accountId, amount, -1);
        if (success)
            res.status = 200;
        else {
            res.status = 400;
            res.set_content("Withdrawal failed", "text/plain");
        }
    });

    // -------------------------
    //  TRANSFER (Protected)
    // -------------------------
    svr.Post("/transaction", [&](const Request& req, Response& res) {
        std::string tokenUserId;
        if (!validate_jwt(req, tokenUserId)) {
            res.status = 401;
            res.set_content("Unauthorized", "text/plain");
            return;
        }
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }
        std::string type = body["type"];
        int accountId = body["accountId"];
        double amount = body["amount"];
        int receiverId = -1;
        // For transfers, if the client sends a "receiverEmail", use that instead of receiverAccountId.
        if (type == "Transfer" && body.contains("receiverEmail")) {
            std::string receiverEmail = body["receiverEmail"];
            UserData* receiverData = db.fetchUserByEmail(receiverEmail);
            if (receiverData == nullptr) {
                res.status = 400;
                res.set_content("Receiver not found", "text/plain");
                return;
            }
            receiverId = getAccountIdByUserId(db, receiverData->user_id);
            delete receiverData;
            if (receiverId == -1) {
                res.status = 400;
                res.set_content("Receiver account not found", "text/plain");
                return;
            }
        } else if (body.contains("receiverAccountId")) {
            receiverId = body["receiverAccountId"].get<int>();
        }
        bool success = api.handleTransaction(type, accountId, amount, receiverId);
        if (success)
            res.status = 200;
        else {
            res.status = 400;
            res.set_content("Transaction failed", "text/plain");
        }
    });

    // -------------------------
    //  TRANSACTION HISTORY
    // -------------------------
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

    // -------------------------
    //  UPDATE PASSWORD
    // -------------------------
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
        if (success)
            res.status = 200;
        else {
            res.status = 400;
            res.set_content("Password update failed", "text/plain");
        }
    });

    // -------------------------
    //  UPDATE PROFILE (Protected)
    // -------------------------
    svr.Post("/update-profile", [&](const Request& req, Response& res) {
        // Validate JWT
        std::string tokenUserId;
        if (!validate_jwt(req, tokenUserId)) {
            res.status = 401;
            res.set_content("Unauthorized", "text/plain");
            return;
        }

        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
            return;
        }
        int userId = body["userId"];
        std::string name = body["name"], email = body["email"];

        // Call the updateProfile function on the user manager.
        // (This assumes you've implemented UserManager::updateProfile)
        bool success = userManager.updateProfile(name, email, userId);
        if (success) {
            res.status = 200;
        } else {
            res.status = 400;
            res.set_content("Profile update failed", "text/plain");
        }
    });

    // -------------------------
    //  ROOT
    // -------------------------
    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("C++ Banking API Running", "text/plain");
    });

    std::cout << "Server is running on http://localhost:3001" << std::endl;
    svr.listen("0.0.0.0", 3001);
    return 0;
}
