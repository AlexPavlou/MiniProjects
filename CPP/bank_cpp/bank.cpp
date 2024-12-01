#include <iostream> 
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sodium.h>
#include <limits>
#include <sqlcipher/sqlite3.h>
#include <sqlite3.h>
#include <random>
#include <memory>
#include <thread>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#ifdef _WIN32
#include <conio.h>  // For Windows-specific _getch()
#else
#include <termios.h>  // For Linux/macOS-specific termios API
#include <unistd.h>
#endif

// Global Variable(s)
int MAX_ACCOUNTS = 3;


// This function is used to receive passwords from the user without showing it on the shell
std::string getPassword() {
    std::string password;
    #ifdef _WIN32
        char ch;
        while (true) {
            ch = _getch();  
            if (ch == '\r') break;  
            password.push_back(ch);
        }
    #else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);  
        newt = oldt;
        newt.c_lflag &= ~ECHO;  
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);  

        char ch;
        while (true) {
            ch = getchar();  
            if (ch == '\n' || ch == '\r') break;  
            password.push_back(ch);
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  
    #endif

        return password;
}

struct Transaction {
	std::string type; // Type of the transaction, can be either 'Withdrawal', 'Deposit', 'Outgoing-Transfer' or 'Incoming-Transfer'
	double amount;
	double balanceAfter;
    int ctp_id; // This variable holds the ID of the 'counterpart', basically the ID of either the sender or recipient in the case of transfers
};

void trimTrailingSpaces(std::string& str) {
    // Use find_if to find the first non-space character from the end
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);  // Stop when a non-whitespace character is found
    }).base(), str.end());  // Erase from the first non-whitespace character to the end
}

class Logger {
public:
    enum LogLevel {
        INFO,
        DEBUG,
        ERROR
    };

    // Constructor that optionally takes a filename to log to
    Logger(const std::string& logFile = "") : logToFile(!logFile.empty()), logFileName(logFile) {
        if (logToFile) {
            logFileStream.open(logFileName, std::ios::app); // Open file in append mode
            if (!logFileStream.is_open()) {
                std::cerr << "Failed to open log file: " << logFileName << std::endl;
                logToFile = false;
            }
        }
    }

    // Destructor ensures the log file stream is closed
    ~Logger() {
        if (logToFile && logFileStream.is_open()) {
            logFileStream.close();
        }
    }

    // Log message with specified log level
    void log(LogLevel level, const std::string& message) {
        std::string levelStr = logLevelToString(level);
        std::string logMessage = getCurrentTime() + " [" + levelStr + "] " + message;

        if (logToFile) {
            if (logFileStream.is_open()) {
                logFileStream << logMessage << std::endl;
            }
        }
    }

private:
    std::ofstream logFileStream;
    bool logToFile;
    std::string logFileName;

    // Convert log level to string
    std::string logLevelToString(LogLevel level) {
        switch (level) {
        case INFO: return "INFO";
        case DEBUG: return "DEBUG";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
        }
    }

    // Get the current timestamp as a string
    std::string getCurrentTime() {
        auto now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream timeStream;
        timeStream << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        return timeStream.str();
    }
};

/* High-Level logging functions to log Debug and Error messages to the db.log file */

void logDebug(const std::string srt){
        Logger logger("db.log");
        logger.log(Logger::DEBUG, srt);
}

void logError(sqlite3* db = nullptr, std::string_view before_str = "", std::string_view errMsg = ""){
        Logger logger("db.log");
        std::stringstream errorMsg;
        if(db!=nullptr){
            errorMsg << before_str << errMsg;
        } else {
            errorMsg << before_str;
        }
        logger.log(Logger::ERROR, errorMsg.str());
}

void saveTransaction(sqlite3* db, const std::string& username, const Transaction& trans, const int& account_id) {
    double amount = trans.amount;
    std::ostringstream stream;
    std::string type = trans.type;
    stream << ", Amount: " << std::fixed << std::setprecision(2) << trans.amount 
           << " €, Balance After: " << std::fixed << std::setprecision(2) << trans.balanceAfter << " €";

    if (trans.type == "Outgoing-Transfer") {
        stream << ", To: " << trans.ctp_id;
    } else if (trans.type == "Incoming-Transfer") {
        stream << ", From: " << trans.ctp_id;
    }

    std::string transaction_details = stream.str();

    // SQL query for inserting the transaction into the database
    const char* insert_sql = "INSERT INTO transactions (type, trans, account_id, amount) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing insert statement for transaction: ", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC); // Bind the transaction details
    sqlite3_bind_text(stmt, 2, transaction_details.c_str(), -1, SQLITE_STATIC); // Bind the transaction description
    sqlite3_bind_int(stmt, 3, account_id);  // Bind the unique account_id
    sqlite3_bind_double(stmt, 4, amount);  // Set the balance value

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        logError(db, "Error inserting transaction into database: ", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);
}

std::string hashPassword(const std::string& password) {
    // Define a buffer to hold the hashed password (it's a fixed size in libsodium)
    char hashedPassword[crypto_pwhash_STRBYTES];

    // Hash the password using libsodium's password hashing function
    if (crypto_pwhash_str(hashedPassword, password.c_str(), password.size(),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        throw std::runtime_error("Password hashing failed");
    }

    // Return the hashed password as a std::string
    return std::string(hashedPassword);
}

bool initialize_sodium() {
    if (sodium_init() == -1) {
        logError(nullptr, "libsodium initialization failed!", "");
        return false;
    }
    return true;
}

bool hash_password(const std::string& password, std::string& hashed_password) {
    const size_t password_len = password.size();
    const size_t hashed_len = crypto_pwhash_STRBYTES;

    // Allocate memory for the hashed password
    hashed_password.resize(hashed_len);

    // Perform password hashing with a high work factor
    if (crypto_pwhash_str(&hashed_password[0], password.c_str(), password_len,
                          crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0) {
        logError(nullptr, "In function hash_password(): Error hashing the password", "");
        return false;
    }

    return true;
}

bool insert_user(sqlite3* db, const std::string& username, const std::string& password, std::string& email, bool& two_fa) {
    if (username.empty()) {
        logError(nullptr, "In function insert_user(): Username cannot be empty!", "");
        return false;
    }

    // Hash the password before storing it
    std::string hashed_password;
    if (!hash_password(password, hashed_password)) {
        return false;
    }

    // Start a transaction for insert
    const char* begin_sql = "BEGIN TRANSACTION;";
    sqlite3_exec(db, begin_sql, nullptr, nullptr, nullptr);

    const char* sql = "INSERT INTO users (username, password, email, two_fa_enabled) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing INSERT statement in function insert_user: ", sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);  // Rollback in case of error
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, two_fa ? 1 : 0);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logError(db, "Error inserting user in function insert_user(): ", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr); // Rollback in case of error
        return false;
    }

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    sqlite3_int64 customer_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    return true;
}

class Account {
	private:
        sqlite3* db;
		std::string username;
		double balance;
        int max_daily_trans;
        int customer_id;
        int ID;
	public:
		Account(std::string user, double user_balance, double trans_limit, int user_id, int id): username(user), balance(user_balance), max_daily_trans(trans_limit), customer_id(user_id), ID(id) {}
        std::string getUsername() const {
            return username;
        }
        void updateUsername(std::string new_username) {
            username = new_username;
        }
		double getBalance() const {
			return balance;
		}
        void updateBalance(double new_balance) {
            balance = new_balance;
        }
        int getID() {
            return ID;
        }
        int getCustomerID() {
            return customer_id;
        }
        double getTransLimit() const {
            return max_daily_trans;
        }
        void setTransLimit(double trans_limit) {
            max_daily_trans = trans_limit;
        }
};

void printTransactionHistory(sqlite3* db, std::shared_ptr<Account> account) {
    sqlite3_stmt* stmt;
    const char* select_transactions_sql = "SELECT trans, type, date FROM transactions WHERE account_id = ? ORDER BY id DESC;";

    // Prepare the SQL statement to select transactions
    if (sqlite3_prepare_v2(db, select_transactions_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing transaction query in function printTransactionHistory(): ", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, account->getID());

    // Execute the query and loop through the results
    int row_count = 0;
    std::cout << " Transactions for account: " << account->getUsername() << ", with ID: " << account->getID() << std::endl;
    std::cout << " Balance: " << std::fixed << std::setprecision(2) << account->getBalance() << "€\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve the transaction and timestamp from the result set
        const unsigned char* transaction = sqlite3_column_text(stmt, 0);  // 'trans' column
        const unsigned char* trans_type = sqlite3_column_text(stmt, 1);         // 'trans_type' column
        const unsigned char* date = sqlite3_column_text(stmt, 2);         // 'date' column

        // Print the transaction details with date
        std::cout << " " << row_count + 1 << ") Type: " << trans_type << transaction << ", (Date: " << date << ")" << std::endl;
        row_count++;
    }

    if (row_count == 0) {
        std::cout << " !No transactions found for this user and account." << std::endl;
    }
    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
}

double calculateDailyTrans(sqlite3* db, std::shared_ptr<Account> acc) {
    double total_trans = 0.0;
    sqlite3_stmt* stmt;

    // SQL query to calculate the sum of today's withdrawals and outgoing transfers
    const char* select_transactions_sql =
        "SELECT IFNULL(SUM(amount), 0) FROM transactions "
        "WHERE account_id = ? "
        "AND date(Date) = date('now', 'utc') "
        "AND (type = 'Withdrawal' OR type = 'Outgoing-Transfer');";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, select_transactions_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing statement in calculateDailyTrans(): ", sqlite3_errmsg(db));
        return -1.0;  // Return an error value
    }

    // Bind the account_id to the prepared statement
    sqlite3_bind_int(stmt, 1, acc->getID());

    // Execute the query and fetch the result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total_trans = sqlite3_column_double(stmt, 0);
    } else {
        logError(db, "Error executing statement in calculateDailyTrans(): ", sqlite3_errmsg(db));
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    logDebug("Daily transactions spent: " + std::to_string(total_trans));
    return total_trans;
}

/*double calculateDailyTrans(sqlite3* db, std::shared_ptr<Account> acc) {
    double total_trans = 0.0;
    sqlite3_stmt* stmt;

    // Updated SQL query with SUM(amount)
    const char* select_transactions_sql =
        "SELECT SUM(amount) FROM transactions "
        "WHERE account_id = ? "
        "AND date(Date) = date('now', 'utc') "
        "AND (type = 'Withdrawal' OR type = 'Outgoing-Transfer');";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, select_transactions_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing statement in function calculateDailyTrans(): ", sqlite3_errmsg(db));
        return -1.0;  // Return an error value
    }

    // Bind the account_id to the prepared statement
    sqlite3_bind_int(stmt, 1, acc->getID());

    // Execute the query and get the result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total_trans = sqlite3_column_double(stmt, 0);
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    logDebug(std::to_string(total_trans));
    return total_trans;
}*/

bool updateAccountBalanceInDB(sqlite3* db, double balance, int account_id) {
    const char* updateSQL = "UPDATE accounts SET account_balance = ? WHERE account_id = ?;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        logError(db, "Failed to prepare statement in function updateAccountBalanceInDB: ", sqlite3_errmsg(db));
        return false;  // Return false if the preparation failed
    }

    // Bind the new balance and account_id to the SQL statement
    sqlite3_bind_double(stmt, 1, balance);  // Set the balance value
    sqlite3_bind_int(stmt, 2, account_id);  // Set the account_id value

    // Debug print to check the values being passed
    std::stringstream debugStr;
    debugStr << "Updating balance for account ID: " << account_id << " to: " << balance;
    logDebug(debugStr.str());

    // Execute the update statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        logError(db, "Failed to update account balance in DB in function updateAccountBalanceInDB: ", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;  // Return false if the execution failed
    }

    // Finalize the prepared statement
    sqlite3_finalize(stmt);

    return true;  // Return true if everything is successful
}

bool deposit(sqlite3* db, double amount, bool showMessage, std::shared_ptr<Account> sender, std::shared_ptr<Account> recipient = nullptr) {
    if (amount <= 0) {
        std::cout << " !Deposit amount negative or invalid.\n";
        return false;
    }

    double temp_balance;
    std::string name;
    int account_id;

    if (recipient == nullptr) {
        logDebug("recipient is nullptr");
        // Deposit to sender's own account
        temp_balance = sender->getBalance() + amount;
        name = sender->getUsername();
        account_id = sender->getID();  // Unique account ID from sender
        if (updateAccountBalanceInDB(db, temp_balance, account_id)) {
            sender->updateBalance(temp_balance);
        } else {
            std::cout << " !Error updating balance in database.\n";
            return false;
        }
    } else {
        // Deposit to recipient's account
        temp_balance = recipient->getBalance() + amount;
        name = recipient->getUsername();
        account_id = recipient->getID();  // Unique account ID from recipient
        if (updateAccountBalanceInDB(db, temp_balance, account_id)) {
            recipient->updateBalance(temp_balance);
        } else {
            std::cout << " !Error updating recipient's balance in database.\n";
            return false;
        }
    }

    if (showMessage) {
        std::cout << " Successfully deposited " << amount << "€ to account: " << name << std::endl;
        // Log the transaction
        Transaction deposit = {"Deposit", amount, temp_balance};
        saveTransaction(db, name, deposit, sender->getID());
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard incorrect input
    return true;
}

void waitForUser() {
    std::cout <<" Press any key to continue...";
    char userInput;
    std::cin.get(userInput);
}

bool withdraw(sqlite3* db, double amount, bool showMessage, std::shared_ptr<Account> account) {
    if (account == nullptr) {
        logError(nullptr, "Invalid account pointer in function withdraw(): ", "");
        return false;
    }

    double balance = account->getBalance();

    // Check for valid amount
    if (amount <= 0 || balance < amount) {
        std::cout << " !Insufficient funds or invalid amount. Withdrawal aborted.\n";
        return false;
    }

    double trans_limit = account->getTransLimit();
    int account_id = account->getID();  // Unique account_id

    logDebug("Trans Limit is " + std::to_string(trans_limit));
    // Before processing the withdrawal, check the daily limit
    if (trans_limit > 0.0) {
        double trans_spent = calculateDailyTrans(db, account);  // Calculate total spent today

        // Ensure the withdrawal does not exceed the daily transaction limit
        int diff = trans_limit - trans_spent;
        if ( amount > diff ) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear any extra input left in buffer
            if ( diff != 0.0 ) {
                std::cout << " !You have set a daily transaction limit of " << trans_limit << " €. You can only spend " << diff << " €.\n";
            } else {
                std::cout << " !You have set a daily transaction limit of " << trans_limit << " €.\n";
            }
            return false;  // Block the transaction if the limit would be exceeded
        }
    }

    // Proceed with the withdrawal if no limit is exceeded
    double temp_balance = balance - amount;  // New balance after withdrawal

    // Update the balance in the database and the account object
    if (updateAccountBalanceInDB(db, temp_balance, account_id)) {
        account->updateBalance(temp_balance);  // Update in-memory balance
    } else {
        std::cout << " !Error updating balance in database.\n";
        return false;
    }

    // **Force Commit the Transaction** here, after balance update
    // This ensures that the balance and transaction are committed before querying again.
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);  // Commit any pending transactions.

    // Log the transaction if required
    if (showMessage) {
        std::cout << " Successfully withdrew " << amount << " € from account: " << account->getUsername() << std::endl;
        Transaction withdrawal = {"Withdrawal", amount, temp_balance};
        saveTransaction(db, account->getUsername(), withdrawal, account->getID());
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard incorrect input
    return true;
}

double getBalanceFromDB(sqlite3* db, int account_id) {
    sqlite3_stmt* stmt;
    double balance = 0.0;

    // SQL query to retrieve the account balance from the database
    const char* select_balance_sql = "SELECT account_balance FROM accounts WHERE account_id = ?;";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, select_balance_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing statement in getBalanceFromDB(): ", sqlite3_errmsg(db));
        return -1.0;  // Return an error value
    }

    // Bind the account_id to the prepared statement
    sqlite3_bind_int(stmt, 1, account_id);

    // Execute the query and retrieve the balance
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        balance = sqlite3_column_double(stmt, 0);
    } else {
        logError(db, "Account not found in getBalanceFromDB(): ", "No rows returned.");
        balance = -1.0;  // Return an error value if no account was found
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return balance;
}

bool transfer(sqlite3* db, double amount, std::shared_ptr<Account> recipient, std::shared_ptr<Account> sender) {
    if (amount <= 0) {
        std::cout << " !Transfer amount must be greater than zero.\n";
        return false;
    }

    // First withdraw from sender's account
    if (!withdraw(db, amount, false, sender)) {
        std::cout << " !Transfer failed due to withdrawal issue.\n";
        return false;
    }

    // Now deposit into recipient's account
    deposit(db, amount, false, sender, recipient);

    // Log the transactions only if both actions are successful
    Transaction OutgoingTransferTransaction = {"Outgoing-Transfer", amount, sender->getBalance(), recipient->getID()};
    saveTransaction(db, sender->getUsername(), OutgoingTransferTransaction, sender->getID());

    Transaction IncomingTransferTransaction = {"Incoming-Transfer", amount, recipient->getBalance(), sender->getID()};
    saveTransaction(db, recipient->getUsername(), IncomingTransferTransaction, recipient->getID());
    return true;
}

void clearScreen() {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    std::cout << "\033[2J\033[1;1H"; // ANSI escape codes for Unix-based systems
#endif
}

/*void clearScreen() {
    std::cout<<std::flush;
    #ifdef _WIN32  // Windows
        system("cls");
    #elif __unix__  // Linux or Unix-like systems
        system("clear");
    #endif
}*/

void displayAccounts(sqlite3* db, std::vector<std::shared_ptr<Account>>& accounts) {
    std::cout <<" Accounts: \n";
    char opt;
    if(accounts.empty()){std::cout<<" !No available accounts, please create an account first.";return;}
    for(int i=0;i<accounts.size();++i){
        std::cout << "  " << i+1 << ") Account name: " << accounts[i]->getUsername() << ", Balance: " << accounts[i]->getBalance() << " €, Account ID: " << accounts[i]->getID() << '\n';
    }
    std::cout << " q) Exit\n";
    std::cout << " c) Clear Screen\n";
    if(accounts.size()==1){std::cout << " Choose an option: ";}
    else {std::cout << " Choose an option (1-" << accounts.size() << "): ";}
}

void removeAccount(sqlite3* db, std::vector<std::shared_ptr<Account>>& accounts, int& i) {

    // Step 6: Update the username in the database
    sqlite3_stmt* stmt;
    const char* removal_sql = "DELETE FROM accounts WHERE account_id = ?";
    
    if (sqlite3_prepare_v2(db, removal_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing update statement in function removeAccount(): ", sqlite3_errmsg(db));
        return;
    }

    int id = accounts[i]->getID();
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        logError(db, "Error removing username in the database in function removeAccount(): ", sqlite3_errmsg(db));
    } else {
        std::cout << " Account removed successfully!" << std::endl;
        logDebug("Removed account with ID: " + std::to_string(id));
        accounts.erase(accounts.begin() + i);
    }

    sqlite3_finalize(stmt);
}

void renameAccount(sqlite3* db, std::vector<std::shared_ptr<Account>>& accounts, int& i) {
    std::string new_username;
    std::cout << " Input new username: ";
    std::getline(std::cin, new_username);
    trimTrailingSpaces(new_username);
    
    // Validate the new username is not empty
    if (new_username.empty()) {
        logError(nullptr, "Invalid username, username cannot be null or empty.", "");
        return;
    }

    for (const auto& acc : accounts){
        if (acc->getUsername() == new_username){
            std::cout << " !Invalid account name. Username already exists. Process aborted.\n";
            break;
        }
    }

    sqlite3_stmt* stmt;
    const char* update_sql = "UPDATE accounts SET username = ? WHERE username = ? AND customer_id = ?";
    
    if (sqlite3_prepare_v2(db, update_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing update statement in function renameAccount(): ", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_text(stmt, 1, new_username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, accounts[i]->getUsername().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, accounts[i]->getCustomerID());

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        logError(db, "Error updating username in the database in function renameAccount(): ", sqlite3_errmsg(db));
    } else {
        std::cout << " Account renamed successfully!" << std::endl;

        // Step 7: Update the in-memory account's username in the vector
        accounts[i]->updateUsername(new_username);
    }

    sqlite3_finalize(stmt);
}

bool checkAccountIdUnique(sqlite3* db, int account_id) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT COUNT(*) FROM accounts WHERE account_id = ?;";
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        logError(db, "Failed to prepare statement in function checkAccountIdUnique(): ", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_int(stmt, 1, account_id);  // Bind the account_id to the query

    rc = sqlite3_step(stmt);
    bool isUnique = false;
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        isUnique = (count == 0);  // If count is 0, the account_id is unique
    }

    sqlite3_finalize(stmt);  // Finalize the statement to free memory
    return isUnique;
}

int generateAccountID(sqlite3* db) {
    int account_id;
    bool isUnique = false;
    
    while (!isUnique) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(100000000, 999999999);  // Range for 9-digit number
        account_id = dis(gen);
        isUnique = checkAccountIdUnique(db, account_id);
    }

    return account_id;
}

bool isValidInput(char& input, int lower_limit, int upper_limit) {
    if (input == '\n' || input == '\0' || input == ' ') {
        return false;
    }

    // Check if the input is a valid digit (between '0' and '9')
    if (!std::isdigit(input)) {
        std::cin.clear();  // Clear the error state
        return false;
    }

    // Convert char to integer (e.g., '1' becomes 1)
    int num = input - '0';

    // Check if the number is within the specified range
    if (num < lower_limit || num > upper_limit) {
        return false;
    }

    return true;  // Return true if the input is valid
}

int getCustomerIDByUsername(sqlite3* db, const std::string& username) {
    const char* query = "SELECT customer_id FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int customer_id = -1;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Failed to prepare statement in getCustomerIDByUsername: ", sqlite3_errmsg(db));
        return -1; // Return -1 if an error occurs
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        customer_id = sqlite3_column_int(stmt, 0); // Fetch the customer_id
    } else {
        std::cout << " !User not found.\n";
    }

    sqlite3_finalize(stmt);
    return customer_id;
}

void create_account(sqlite3* db, std::vector<std::shared_ptr<Account>>& accounts, const std::string& customer_name) {
    if (accounts.size() == MAX_ACCOUNTS) {
        std::cout << " !You have reached the maximum number of accounts. Process aborted." << std::endl;
        return;
    }

    std::string username;
    int customer_id = getCustomerIDByUsername(db, customer_name);
    std::cout << " New account username: ";
    std::getline(std::cin, username);
    trimTrailingSpaces(username);
    if (std::cin.fail()) {
        std::cin.clear();  // Clear input failure
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard extra characters
    }

    // Validate the username
    if (username.empty()) {
        logError(nullptr, "Invalid username: Username cannot be null or empty in function create_account()", "");
        return;
    }

    // Check if the account already exists (if desired)
    for (int i = 0; i < accounts.size(); ++i) {
        if (accounts[i]->getUsername() == username) {
            std::cout << " !You already own an account with this name." << std::endl;
            return;
        }
    }

    // Manually generate random account_id
    int account_id = generateAccountID(db); // Call your custom account ID generator function

    // Prepare the insert SQL statement
    const char* insert_sql = "INSERT INTO accounts (account_id, customer_id, username, account_balance, trans_limit) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* insert_stmt;

    if (sqlite3_prepare_v2(db, insert_sql, -1, &insert_stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing insert statement in function create_account(): ", sqlite3_errmsg(db));
        sqlite3_finalize(insert_stmt);
        return;
    }

    double initial_balance = 0.0;
    std::cout << " Enter initial balance for the account: ";
    std::cin >> initial_balance;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear any extra input left in buffer

    if (initial_balance <= 0) {
        logError(nullptr, "Invalid input: The balance must be positive and non-zero in function create_account()", "");
        sqlite3_finalize(insert_stmt);
        return;
    }

    // Ask for transaction limit
    double trans_limit = 0.0;
    std::string trans_limit_option;
    std::cout << " Would you like to set a daily transaction limit? ";
    std::cin >> trans_limit_option;
    if (trans_limit_option.empty()) return;  // Exit if no option was provided
    if (trans_limit_option[0] == 'y' || trans_limit_option[0] == 'Y') {
        std::cout << " Set daily transaction limit (input 0 for no limit): ";
        std::cin >> trans_limit;
        if (trans_limit < 0) {
            std::cout << " Transaction limit cannot be negative. Account creation aborted." << std::endl;
            return;
        }
    }

    // Bind parameters for account creation
    sqlite3_bind_int(insert_stmt, 1, account_id);  // Bind account_id (generated by you)
    sqlite3_bind_int(insert_stmt, 2, customer_id);     // Bind customer_id (retrieved from the users table)
    sqlite3_bind_text(insert_stmt, 3, username.c_str(), -1, SQLITE_STATIC);  // Bind username
    sqlite3_bind_double(insert_stmt, 4, initial_balance);  // Bind initial balance
    sqlite3_bind_double(insert_stmt, 5, trans_limit);    // Bind transaction limit

    // Execute the insert statement to create the account
    if (sqlite3_step(insert_stmt) != SQLITE_DONE) {
        logError(db, "Error inserting new account in function create_account(): ", sqlite3_errmsg(db));
    } else {
        std::cout << " New account created successfully!" << std::endl;
        logDebug("Account '" + username + "' created successfully (function create_account())");

        // Add the account to the accounts vector
        accounts.push_back(std::make_shared<Account>(username, initial_balance, trans_limit, customer_id, account_id));

        // Sort the accounts by username in descending order
        std::sort(accounts.begin(), accounts.end(), [](const std::shared_ptr<Account>& a, const std::shared_ptr<Account>& b) {
            return std::string(a->getUsername()) > std::string(b->getUsername());
        });

        // If the initial balance is greater than 0, save the initial deposit transaction
        if (initial_balance > 0) {
            Transaction deposit = {"Initial-Deposit", initial_balance, initial_balance};
            saveTransaction(db, username, deposit, account_id);
        }
    }
    sqlite3_finalize(insert_stmt);
}

void fetchAccountsByUserID(sqlite3* db, const int& customer_id, std::vector<std::shared_ptr<Account>>& accounts) {
    sqlite3_stmt* stmt;

    const char* account_sql = "SELECT account_id, username, account_balance, trans_limit FROM accounts WHERE customer_id = ?";
    
    if (sqlite3_prepare_v2(db, account_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing SELECT statement for accounts: ", sqlite3_errmsg(db));
        return;
    }

    // Bind the customer_id to the account query
    sqlite3_bind_int(stmt, 1, customer_id);

    // Execute the query and fetch the accounts
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int account_id = sqlite3_column_int(stmt, 0);
        const unsigned char* account_username = sqlite3_column_text(stmt, 1);
        double account_balance = sqlite3_column_double(stmt, 2);
        double trans_limit = sqlite3_column_double(stmt, 3);

        // Create a new Account object with the fetched details
        accounts.push_back(std::make_shared<Account>(reinterpret_cast<const char*>(account_username), account_balance, trans_limit, customer_id, account_id));
        // Sort the accounts by username in descending order
        std::sort(accounts.begin(), accounts.end(), [](const std::shared_ptr<Account>& a, const std::shared_ptr<Account>& b) {
            return std::string(a->getUsername()) > std::string(b->getUsername());
        });
    }

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
}

// Function to initialize the database and create the table (if not exists)
int createTables(sqlite3* db) {
        const char* create_users_sql = R"(CREATE TABLE IF NOT EXISTS users (
    customer_id INTEGER PRIMARY KEY AUTOINCREMENT,
    password TEXT NOT NULL,
    email TEXT NOT NULL,
    two_fa_enabled BOOLEAN DEFAULT FALSE,
    username VARCHAR(255) NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP);
    )";

    const char* create_accounts_sql = R"(CREATE TABLE IF NOT EXISTS accounts (
    account_id INTEGER PRIMARY KEY,
    customer_id INTEGER NOT NULL,
    username TEXT NOT NULL,
    account_balance REAL NOT NULL,
    trans_limit REAL NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (account_id) REFERENCES users(account_id) ON DELETE CASCADE);
    )";

    // Create the 'transactions' table if it doesn't exist
    const char* create_transactions_sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT NOT NULL,
            trans TEXT NOT NULL,
            account_id INTEGER NOT NULL,
            amount INTEGER NOT NULL,
            Date DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    // Create index on account_id in accounts table
    const char* create_accounts_index_sql = R"(CREATE INDEX IF NOT EXISTS idx_account_id ON accounts(account_id);)";

    char* errorMessage;
    
    // Execute the SQL statements
    if (sqlite3_exec(db, create_users_sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        logError(db, "Error creating users table in function createTables(): " + std::string(errorMessage), sqlite3_errmsg(db));
        sqlite3_free(errorMessage);
        return 1;
    }

    if (sqlite3_exec(db, create_accounts_sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        logError(db, "Error creating accounts table in function createTables(): " + std::to_string(*errorMessage), sqlite3_errmsg(db));
        sqlite3_free(errorMessage);
        return 1;
    }

    if (sqlite3_exec(db, create_transactions_sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        logError(db, "Error creating transactions table in function createTables(): " + std::to_string(*errorMessage), sqlite3_errmsg(db));
        sqlite3_free(errorMessage);
        return 1;
    }

    // Create index on account_id in accounts table
    if (sqlite3_exec(db, create_accounts_index_sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        logError(db, "Error creating index on account_id in function createTables(): " + std::to_string(*errorMessage), sqlite3_errmsg(db));
        sqlite3_free(errorMessage);
        return 1;
    }

    return 0;
}

bool find_user(sqlite3* db, const std::string& username, std::string& password_hash) {
    sqlite3_stmt* stmt;
    
    // SQL query to fetch the password hash for the given username
    const char* sql = "SELECT password FROM users WHERE username = ?";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        logError(db, "Error preparing SQL statement: in function find_user()", sqlite3_errmsg(db));
        return false;
    }

    // Bind the username to the SQL statement
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Execute the query and fetch the result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // If the query was successful, retrieve the password hash
        password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return true;
    } else {
        sqlite3_finalize(stmt);
        return false;
    }
}

// Function to verify the entered password with the hashed password from the database
bool verify_password(const std::string& password, const std::string& hashed_password) {
    // Verify the entered password with the stored hashed password
    return crypto_pwhash_str_verify(hashed_password.c_str(), password.c_str(), password.size()) == 0;
}

double getAmount( std::string str) {
    double amount;
    std::cout<<" Please input the amount of money you wish to " << str << ": ";
    std::cin >> amount;
    if (!std::cin.fail() && amount > 0) {  // If input is an integer
        return amount;  
    }
    if (amount<0){
        std::cout<< " !Negative amounts are not allowed. Operation aborted";
    } else { std::cout << " !Invalid input. Operation aborted\n"; }
    return -1;
}

std::shared_ptr<Account> lookupRecipient(sqlite3* db, const int& recipient_id) {
    sqlite3_stmt* stmt; 

    // Query to find the account based on account_id, including the customer_id
    const char* select_sql = "SELECT username, account_balance, customer_id FROM accounts WHERE account_id = ?";
    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr) != SQLITE_OK) { 
        logError(db, "Error preparing query in function lookupRecipient: ", sqlite3_errmsg(db));
        return nullptr; 
    } 
    
    sqlite3_bind_int(stmt, 1, recipient_id); 

    int result = sqlite3_step(stmt);

    if (result == SQLITE_ROW) {
        const unsigned char* account_username_raw = sqlite3_column_text(stmt, 0);  // Raw username (unsigned char*)
        double account_balance = sqlite3_column_double(stmt, 1);  // Fetch account_balance
        int customer_id = sqlite3_column_int(stmt, 2);  // Fetch customer_id directly from the query

        // Convert the raw username to std::string
        std::string account_username(reinterpret_cast<const char*>(account_username_raw));

        // Finalize the statement
        sqlite3_finalize(stmt); 

        // Create and return the Account object
        return std::make_shared<Account>(account_username, account_balance, false, customer_id, recipient_id);
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

void displayMainMenu(const std::string& customer_name, const std::vector<std::shared_ptr<Account>>& accounts) {
    clearScreen();

    int maxlen = 0;
    for (const auto& account : accounts) {
        maxlen = std::max(maxlen, static_cast<int>(account->getUsername().length()));
    }

    std::string header_text = " Accounts for: " + customer_name + " ";
    int len = std::max(static_cast<int>(header_text.length()), 32); // Minimum width of 32
    maxlen = std::max(maxlen + 20, len); // Ensure sufficient spacing for account details
    len = maxlen;

    int total_dashes = len - static_cast<int>(header_text.length());
    int dashes_before = total_dashes / 2;
    int dashes_after = total_dashes - dashes_before;

    std::cout << " " << std::string(dashes_before, '-') << header_text << std::string(dashes_after, '-') << '\n';
    std::cout << " " << std::string(len, '-') << '\n';

    if (!accounts.empty()) {
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (i > 0) {
                std::cout << std::string(len / 4, ' ') << std::string(len / 2, '-') << std::endl;;
            }
            std::cout << "  Account Username: " << accounts[i]->getUsername();
            std::cout << "\n  Balance: " << accounts[i]->getBalance() << " €";
            std::cout << "\n  Account ID: " << accounts[i]->getID() << std::endl;
        }
    } else {
        // Center "No accounts found for this user" on a single line
        std::string no_account_msg = "No accounts found for this user";
        int spaces_to_center = (len - static_cast<int>(no_account_msg.length())) / 2;
        std::cout << "  " << std::string(spaces_to_center, ' ') << no_account_msg << std::endl;
    }

    std::cout << " " << std::string(len, '-') << '\n';

    // Center "--- Main Menu ---"
    std::string main_menu_header = "--- Main Menu ---";
    int menu_header_padding = (len - static_cast<int>(main_menu_header.length())) / 2;
    std::cout << " " << std::string(menu_header_padding, ' ') << main_menu_header << '\n';

    // Align options to the left
    std::cout << " 1) Deposit\n";
    std::cout << " 2) Withdraw\n";
    std::cout << " 3) Transfer\n";
    std::cout << " 4) Transaction History\n";
    std::cout << " 5) Create New Account\n";
    std::cout << " 6) Rename Account\n";
    std::cout << " 7) Remove Account\n";
    std::cout << " q) Log Out/Exit\n";
    std::cout << " c) Clear Screen\n";
    std::cout << " Choose an option (1-7): ";
}

void handleUserInput(sqlite3* db, std::vector<std::shared_ptr<Account>>& accounts, bool& flag_finished, std::string customer_name) {
    char opt;
    while (true) {
        displayMainMenu(customer_name, accounts);   
        std::cin >> opt;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear any extra input left in buffer
        if (opt == 'q') {
            clearScreen();
            return;
        }
        
        if (opt == 'c') {
            clearScreen();
            continue;  // After clearing screen, we continue the loop, no need to wait for additional input
        }

        while (!isValidInput(opt, 1, 7)) {
            displayMainMenu(customer_name, accounts);   
            std::cin >> opt;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear any extra input left in buffer
            
            if (opt == 'q') {
                clearScreen();
                return;
            }
            if (opt == 'c') {
                clearScreen();
                continue;  // After clearing screen, we continue the loop, no need to wait for additional input
            }
        }
        if (opt!='5' && accounts.size() == 0) {
            std::cout << " You have no accounts, please create an account first.\n";
            waitForUser();
            continue;
        }
        switch (opt) {
            case '1': {
                double amount = getAmount("deposit");
                if (amount > 0) {
                    displayAccounts(db, accounts);
                    char option = '0';
                    while (true) {
                        std::cin >> option;
                        
                        if (option == 'q') {
                            clearScreen();
                            break;
                        }
                        if (option == 'c') {
                            clearScreen();
                            displayAccounts(db, accounts);  // Re-display accounts after clearing screen
                            continue;
                        }

                        if (isValidInput(option, 1, accounts.size())) {
                            int index = option - '1';
                            if (index < accounts.size()) {
                                if(deposit(db, amount, true, accounts[index])) {
                                    std::cout<<" Deposit completed successfully!" << std::endl;
                                }
                                break;  // Exit after successful deposit
                            }
                        } else {
                            std::cout << " !Invalid input. Please choose a valid option.";
                            displayAccounts(db, accounts);
                        }
                    }
                }
                break;
            }
            case '2': {
                double amount = getAmount("withdraw");
                if (amount > 0) {
                    displayAccounts(db, accounts);
                    char option = '0';
                    while (true) {
                        std::cin >> option;
                        
                        if (option == 'q') {
                            clearScreen();
                            break;
                        }
                        if (option == 'c') {
                            clearScreen();
                            displayAccounts(db, accounts);  // Re-display accounts after clearing screen
                            continue;
                        }

                        if (isValidInput(option, 1, accounts.size())) {
                            int index = option - '1';
                            if (index < accounts.size()) {
                                if(withdraw(db, amount, true, accounts[index])) {
                                    std::cout<<" Withdrawal completed successfully!" << std::endl;
                                }
                                break;  // Exit after successful withdrawal
                            }
                        } else {
                            std::cout << " !Invalid input. Please choose a valid option.\n";
                            waitForUser();
                            displayAccounts(db, accounts);
                        }
                    }
                }
                break;
            }
            case '3': {
                double amount = getAmount("transfer");
                int recipient_id;
                char opt = '0';
                if (amount <= 0) { continue; }
                while (!isValidInput(opt, 1, accounts.size())) {
                    displayAccounts(db, accounts);
                    std::cin >> opt;
                    std::cin.clear();
                    if (opt == 'q') { clearScreen(); continue; }
                    if (opt == 'c') { clearScreen(); }
                }

                int index = opt - '1';
                std::cout << " Input the account id you would like to transfer " << amount << "€ to: ";
                std::cin >> recipient_id;
                if (recipient_id <= 0) {
                    std::cerr << " Invalid account id, account id must be a positive integer";
                    continue;
                }
                std::shared_ptr<Account> recipient = lookupRecipient(db, recipient_id);
                if (recipient != nullptr) {
                    if(transfer(db, amount, recipient, accounts[index])){
                        std::cout<<" Transfer completed successfully!" << std::endl;
                        for (const auto& acc : accounts ) {
                            if (acc->getID() == recipient_id) {
                                accounts.clear();
                                fetchAccountsByUserID(db, recipient->getCustomerID(), accounts);
                                break;
                            }
                        }
                    }
                } else {
                    std::cout << " !Invalid recipient username. Transfer aborted.\n";
                }
                break;
            }
            case '4': {
                char option = '0';
                while (true) {
                    displayAccounts(db, accounts);
                    std::cin >> option;
                    if (option == '0') { break; }
                    if (option == 'q') { clearScreen(); break; }
                    if (option == 'c') { clearScreen(); displayAccounts(db, accounts); continue; }

                    if (isValidInput(option, 1, accounts.size())) {
                        int index = option - '1';
                        printTransactionHistory(db, accounts[index]);
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        break;
                    } else {
                        std::cout << " !Invalid input. Please choose a valid option.\n";
                        displayAccounts(db, accounts);
                    }
                }
                break;
            }
            case '5': {
                create_account(db, accounts, customer_name);
                break;
            }
            case '6': {
                int index;
                displayAccounts(db, accounts);
                char option = '0';
                while (true) {
                    std::cin >> option;
                    
                    if (option == 'q') {
                        clearScreen();
                        break;
                    }
                    if (option == 'c') {
                        clearScreen();
                        displayAccounts(db, accounts);
                        continue;
                    }

                    if (isValidInput(option, 1, accounts.size())) {
                        index = option - '1';
                        break;
                    } else {
                        std::cout << " !Invalid input. Please choose a valid option.\n";
                        waitForUser();
                        displayAccounts(db, accounts);
                    }
                }
                renameAccount(db, accounts, index);
                break;
            }
            case '7': {
                int index;
                displayAccounts(db, accounts);
                char option = '0';
                while (true) {
                    std::cin >> option;
                    
                    if (option == 'q') {
                        clearScreen();
                        break;
                    }
                    if (option == 'c') {
                        clearScreen();
                        displayAccounts(db, accounts);
                        continue;
                    }

                    if (isValidInput(option, 1, accounts.size())) {
                        index = option - '1';
                        break;
                    } else {
                        std::cout << " !Invalid input. Please choose a valid option.\n";
                        waitForUser();
                        displayAccounts(db, accounts);
                    }
                }
                removeAccount(db, accounts, index);
                break;
            }
        }
        waitForUser();
    }
}

bool validPass(const std::string& password) {
    bool hasUppercase = false;
    bool hasLowercase = false;
    bool hasDigit = false;
    bool hasSpecialChar = false;
    bool isLongEnough = password.length() >= 6;

    for (char ch : password) {
        if (isupper(ch)) hasUppercase = true;
        if (islower(ch)) hasLowercase = true;
        if (isdigit(ch)) hasDigit = true;
        if (ispunct(ch)) hasSpecialChar = true;
    }

    int result = (hasUppercase && hasLowercase && hasDigit && hasSpecialChar && isLongEnough);
    if(!result){
        std::cout<<"\n !Password is too weak!";
        std::cout<<" A password is considered strong when it has:\n";
        if (!isLongEnough) {
            std::cout << " * At least 6 characters.\n";
        } else {std::cout << " ✓ At least 6 characters.\n";}
        if (!hasUppercase) {
            std::cout << " * At least 1 uppercase character.\n";
        } else {std::cout << " ✓ At least 1 uppercase character.\n";}
        if (!hasLowercase) {
            std::cout << " * At least 1 lowercase character.\n";
        } else {std::cout << " ✓ At least 1 lowercase character.\n";}
        if (!hasDigit) {
            std::cout << " * At least 1 digit.\n";
        } else {std::cout << " ✓ At least 1 digit.\n";}
        if (!hasSpecialChar) {
            std::cout << " * At least 1 special character (e.g., !, @, #, $, %, etc.).\n";
        } else {std::cout << " ✓ At least 1 special character (e.g., !, @, #, $, %, etc.).\n";}
    }
    return result;
}

void create_user(sqlite3* db, bool& finished_flag) {
    std::string customer_name, password, confirmPassword;
    std::cout << " Enter username: ";
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::getline(std::cin, customer_name);
    trimTrailingSpaces(customer_name);
    char option;

    if(customer_name.empty()){std::cout<<" !Username cannot be an empty string.\n";return;}

    std::string existingPasswordHash;
    while (find_user(db, customer_name, existingPasswordHash)) {
        std::cout << " !Username already exists. Choose another username or press 'q' to quit the user creation process: ";
        std::cin.get(option);
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if (option == 'q') { clearScreen(); return; }
        std::cout << " Enter username: ";
        std::getline(std::cin, customer_name);
        trimTrailingSpaces(customer_name);
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    bool pass_flag = false;
    while (!pass_flag) {
        std::cout << " Enter password: ";
        password = getPassword();
        while (!validPass(password)) { std::cout << " Enter Password: "; password = getPassword(); }
        std::cout << "\n Confirm password: ";
        confirmPassword = getPassword();
        if (password != confirmPassword) {
            std::cout << "\n !Passwords do not match. Please try again.\n";
            std::cout << " Press any key to continue, or 'q' to quit the process\n Your option: ";
            std::cin.get(option);
            if (option == 'q') { clearScreen(); return; }
        } else { pass_flag = true; }
    }

    bool two_fa = false;
    std::string email_addr;
    std::cout << "\n Input your email address: ";
    std::cin >> email_addr;

    const std::regex email_pattern(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
    if (!std::regex_match(email_addr, email_pattern)) { std::cout << " !Invalid e-mail address. Account creation aborted\n"; return; }

    // Insert the user into the users table
    insert_user(db, customer_name, password, email_addr, two_fa);

    logDebug("User '" + customer_name + "' created successfully (function create_user())");
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard incorrect input
    std::vector<std::shared_ptr<Account>> accounts;
    std::cout << " User registered successfully"<<std::endl;
    waitForUser();
    handleUserInput(db, accounts, finished_flag, customer_name);
}

// checks if user has surpassed the maximum amount of max_tries 
// this function receives the current try the user is in ( int& tries ), the maximum tries he can use (int& max_tries ) and the duration the lockout should be enforced for
// if he has, the program will ask him to wait for increasing amounts of time (5sec --> 5*3=15seconds --> ...) and will also decrease the max_tries variable by 1 each time, until it reaches its minimum value (which is a single try)
void lockOutLogic( int& tries, int& max_tries, int& duration) {
    if (tries==max_tries+1) {
            for (int i = duration; i>0; --i) {
                clearScreen();
                std::cout << " !Too many incorrect attempts, you can try again in: " << i << " seconds" << std::flush;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            tries=1;
            if (max_tries>1) {
                max_tries-=1;
            }
            duration = std::min(duration * 3, 60);
            clearScreen();
    }
}

// Function to handle login and retrieve account data from SQLite
void request_login(sqlite3* db, bool& finished_flag) {
    std::string customer_name;
    bool logged_in = false;
    int tries = 1;
    int max_tries = 3;
    int duration = 5;
    char option{};
    std::string pass;

    // Loop for handling login attempts
    while (!logged_in) {
        // Implement lockout logic (e.g., blocking further attempts if max_tries is reached)
        lockOutLogic(tries, max_tries, duration);

        // Prompt for username
        std::cout << " Enter username: ";
        std::getline(std::cin, customer_name);
        trimTrailingSpaces(customer_name);
        // Prompt for password
        std::cout << " Enter password: ";
        pass = getPassword(); // Use std::getline to correctly read password with spaces
        std::cout << std::endl; // Ensure new line after password input

        // Retrieve the hashed password from the database
        std::string stored_password_hash;
        if (find_user(db, customer_name, stored_password_hash)) {
            // Verify the provided password with the stored hash
            if (verify_password(pass, stored_password_hash)) {
                logged_in = true;  // Successfully logged in
            } else {
                // Invalid password, let the user try again
                std::cout << " !Invalid Password or username. Try again or press 'q' to quit the login process: ";
                option = std::cin.get();
                if (option == 'q') { clearScreen(); return; }
                tries++;
            }
        } else {
            // Invalid username, let the user try again
            std::cout << " !Invalid Password or username. Try again or press 'q' to quit the login process: ";
            option = std::cin.get();
            if (option == 'q') { clearScreen(); return; }
            tries++;
        }
    }

    // At this point, the user is successfully logged in, so fetch accounts related to this user
    std::vector<std::shared_ptr<Account>> accounts;
    fetchAccountsByUserID(db, getCustomerIDByUsername(db, customer_name), accounts); // Fetch accounts after successful login
    printTransactionHistory(db, accounts[0]);
    // Proceed to handle user input for managing accounts
    handleUserInput(db, accounts, finished_flag, customer_name);  // Pass the fetched accounts for further interaction
}

void request_login_or_create(char& option_login_or_create, bool& finished){
    std::cout << " ---------- Welcome ----------\n";
    std::cout << " Are you:\n";
    std::cout << " 1) An existing User\n";
    std::cout << " 2) A new User\n";
    std::cout << " q) Exit\n";
    std::cout << " Choose an option (1-2): ";
    std::cin >> option_login_or_create;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear any extra input left in buffer
    if(option_login_or_create=='q'){finished=true;clearScreen();return;}
    while(!isValidInput(option_login_or_create,1,2)) {
        std::cout << " ---------- Welcome ----------\n";
        std::cout << " Are you:\n";
        std::cout << " 1) An existing User\n";
        std::cout << " 2) A new User\n";
        std::cout << " q) Exit\n";
        std::cout << " Choose an option (1-2): ";
        std::cin >> option_login_or_create;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Clear any extra input left in buffer
        if(option_login_or_create=='q'){finished=true;clearScreen();return;}
    }
}

void requestAuthOptions(sqlite3* db) {
    char option_login_or_create;
    bool finished = false;
    request_login_or_create(option_login_or_create, finished);
    while(!finished){
        switch (option_login_or_create) {
            case '1':
                request_login(db, finished);
            break;
            case '2':
                create_user(db, finished);
            break;
        }
        request_login_or_create(option_login_or_create, finished);
    }
}

int main () {
    clearScreen();
	// Initialize libsodium
    if (sodium_init() == -1) {
        logError(nullptr, "libsodium initialization failed! (function: int main())","");
        return 1;
    }

    sqlite3 *db;
    const std::string db_name = "user_data.db";  // Use absolute path
    const std::string passphrase = "T!meTo$ecure2024&dAt@b@se#R#0ck";  // Your passphrase (commented out)

    // Open the database (this will create the file if it doesn't exist)
    int rc = sqlite3_open_v2(db_name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    sqlite3_exec(db, ("PRAGMA key = '" + passphrase + "';").c_str(), nullptr, nullptr, nullptr);

    if (createTables(db) != 0) {
        sqlite3_close(db);
        return 1;
    }
    requestAuthOptions(db);

    sqlite3_close(db); // Close the database
    return 0;
}
