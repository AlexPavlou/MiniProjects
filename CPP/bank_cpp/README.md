<h1 align="center" style="font-size: 2em;">Bank.cpp 💳</h1>

> A simple, secure cli banking app with multi-account support, transaction limits, user authentication, logging and multiple security features.

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=flat&logo=c%2B%2B&logoColor=white)
![SQLite](https://img.shields.io/badge/sqlite-%2307405e.svg?style=flat&logo=sqlite&logoColor=white)

<div align="center" style="display: flex; justify-content: space-between; width: 100%;">
  <img src="../../../images/bank/bank-welcome.png" style="width: 48%; object-fit: contain;">
  <img src="../../../images/bank-accounts.png" style="width: 48%; object-fit: contain;">
</div>

## Summary 📝
> This program allows users to:
- Log in and manage multiple bank accounts
- Deposit, withdraw, and transfer funds
- Set daily transaction limits
- View transaction histories per account

## Features ✨
- **User Authentication**: Secure login with password hashing and lock-out protection.
- **Account Management**: Create, rename, remove accounts and view the transaction history.
- **Transaction Limits**: Set daily limits for withdrawals and transfers for effective management.
- **Logging**: Logs debug and error statements for troubleshooting.

## Requirements 📋

To build and run this project, the following libraries are required:
- **C++17 or later**
- **SQLite3**: Used for storing user data and transaction details.
- **SQLcipher**: Used for encrypting the database
- **libsodium**: Used for secure password hashing and verification using the Argon2 algorithm.

## Installing Dependencies ⚙️

### On Ubuntu
```bash
sudo apt update
sudo apt install libsodium-dev sqlite3 libsqlite3-dev sqlcipher libsqlcipher-dev
```

### On macOS
```bash
brew install libsodium sqlite sqlcipher
```

### On Windows
1. Download and install [SQLite](https://sqlite.org/download.html).
2. Download and install [SQLCipher](https://github.com/sqlcipher/sqlcipher).
3. Download and install [libsodium](https://libsodium.gitbook.io/doc/installation).
4. Configure your compiler to include the paths to the downloaded libraries.

## Usage 🚀
1. Clone or download the repository.
2. Compile the program using your preferred compiler (e.g., g++).
   Example:
   
   ```bash
   g++ -o bank bank.cpp -lsqlcipher -lsodium -lpthread -lz
   ```
3. Run the compiled program:

   ```bash
   ./bank
   ```
