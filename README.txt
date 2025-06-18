# 💳 Secure Online Banking System

A full-stack web application that brings enterprise-grade online banking to the classroom.  
It combines a **multithreaded C++17 backend** with **JWT-based authentication** and a modern **React** frontend, supporting ACID-compliant fund transfers, real-time balances, and exhaustive transaction history.

---

## ✨ Core Features

| Domain | Highlights |
|--------|------------|
| **Authentication** | JWT access & refresh tokens, salted/hashed passwords (bcrypt). |
| **Accounts & Users** | CRUD endpoints, role-based access, multi-account support. |
| **Transactions** | Atomic money transfers, double-entry ledger, rollback on failure. |
| **Concurrency** | `std::thread` workers + mutexes ensure safe, parallel request handling. |
| **Database** | SQLite (ACID) with prepared statements; upgradeable to Postgres/MySQL. |
| **Frontend** | React + Context API + Axios; live balances with WebSocket push. |

---

## 🛠️ Tech Stack

| Layer          | Tools / Libraries |
|----------------|-------------------|
| **Backend**    | C++17 · jwt-cpp · SQLite3 · OpenSSL · cpp-httplib |
| **Frontend**   | React · Vite · TailwindCSS |

## 🚀 Getting Started

### 1. Prerequisites

| Platform | Requirements |
|----------|--------------|
| **Windows 10+** | MinGW-w64 (or MSVC) + OpenSSL 3.x |
| **WSL / Linux** | g++ 11+ · libssl-dev · libsqlite3-dev · pthreads |

Clone the repo and make sure submodules are pulled:

```bash
git clone https://github.com/vekaria04/secure-online-banking-system.git
cd online-banking
```

### 2. Backend - Build & Run
#### ▶️ Windows / MinGW
```bash
cd server
g++ -std=c++17 -I./third_party/jwt-cpp/include ^
    Main.cpp Account.cpp APIController.cpp Transaction.cpp ^
    TransactionManager.cpp User.cpp UserManager.cpp ^
    -o seead -lws2_32 -lssl -lcrypto
.\seead
```
#### ▶️ WSL / Linux
```bash
cd server
g++ -std=c++17 -I./third_party/jwt-cpp/include \
    Main.cpp Account.cpp APIController.cpp Transaction.cpp \
    TransactionManager.cpp User.cpp UserManager.cpp DatabaseConnector.cpp \
    -o server -lssl -lcrypto -lpthread -lsqlite3
./server
```

Environment variables
JWT_SECRET, PORT (default 8080), and DB_PATH (default ./bank.db) can be exported before running.

### 3. Frontend
```bash
cd client
npm install
npm run dev
```


