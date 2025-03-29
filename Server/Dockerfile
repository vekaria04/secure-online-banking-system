# Use an official C++ base image
FROM gcc:latest

# Install dependencies (SSL, SQLite3)
RUN apt-get update && apt-get install -y libssl-dev libsqlite3-dev

# Create app directory
WORKDIR /app

# Copy all source code to container
COPY . .

# Compile the server
RUN g++ -std=c++17 Main.cpp Account.cpp APIController.cpp Transaction.cpp TransactionManager.cpp User.cpp UserManager.cpp DatabaseConnector.cpp -o server -lpthread -lssl -lcrypto -lsqlite3

# Expose the port your server runs on
EXPOSE 8080

# Command to run your server
CMD ["./server"]
