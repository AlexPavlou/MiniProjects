#include <iostream> 
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sqlite3.h>
#include <sodium.h>
#include <limits>
#include <memory>
#include <thread>
#include <cstdlib>
#ifdef _WIN32
#include <conio.h>  // For Windows-specific _getch()
#else
#include <termios.h>  // For Linux/macOS-specific termios API
#include <unistd.h>
#endif

std::string getPassword() {
    std::string password;
    #ifdef _WIN32
        // Windows-specific (using _getch from conio.h)
        char ch;
        while (true) {
            ch = _getch();  // Read a character without echoing
            if (ch == '\r') break;  // End input on Enter key (carriage return)
            password.push_back(ch);
        }
    #else
        // Linux/macOS-specific (using termios API)
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);  // Get current terminal settings
        newt = oldt;
        newt.c_lflag &= ~ECHO;  // Disable echoing
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Set new terminal settings

        char ch;
        while (true) {
            ch = getchar();  // Read a character
            if (ch == '\n' || ch == '\r') break;  // End input on newline or carriage return
            password.push_back(ch);
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore original terminal settings
    #endif

        return password;
}

struct Transaction {
	std::string type;
	double amount;
	double balanceAfter;
    std::string ctp; // counterpart, contains the name of the account that sent you funds, or the name of the account to whom you have sent funds
	std::string timestamp;
};

/**
 * returns the current timestamp
 *
 * This function is used to note when a transaction has happened, it receives no parameters and returns a string that contains the current timestamp in the format of "%d-%m-%Y %H:%M:%S" (day-month-Year), e.g. (17-11-2024 04:58:23)
 *
 * @return      string
 */
std::string getCurrentTimestamp() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	std::tm local_tm = *std::localtime(&now_time);
	std::ostringstream oss;
	oss << std::put_time(&local_tm, "%d-%m-%Y %H:%M:%S");
	return oss.str();
}

void saveTransaction(sqlite3* db, const std::string& username, const Transaction& trans) {
    // 1. Format the transaction into a string
    std::ostringstream stream;
    if(trans.type=="Outgoing-Transfer") {
        stream << "Type: " << trans.type << ", Amount: " << std::fixed << std::setprecision(2) << trans.amount << trans.balanceAfter << ", To: " << trans.ctp << ", Timestamp: " << trans.timestamp;
    } else if (trans.type=="Incoming-Transfer") {
        stream << "Type: " << trans.type << ", Amount: " << std::fixed << std::setprecision(2) << trans.amount << trans.balanceAfter << ", From: " << trans.ctp << ", Timestamp: " << trans.timestamp;
    } else {
        stream << "Type: " << trans.type << ", Amount: " << std::fixed << std::setprecision(2) << trans.amount << trans.balanceAfter << ", Timestamp: " << trans.timestamp;
    }
    std::string transaction_details = stream.str();

    // 2. Insert the transaction into the transactions table
    const char* insert_sql = "INSERT INTO transactions (username, trans) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing insert statement for transaction: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind the username and transaction details to the insert statement
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC); // Bind the username
    sqlite3_bind_text(stmt, 2, transaction_details.c_str(), -1, SQLITE_STATIC); // Bind the transaction details

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Error inserting transaction into database: " << sqlite3_errmsg(db) << std::endl;
    }

    // Finalize the statement to free resources
    sqlite3_finalize(stmt);
}


void printTransactionHistory(sqlite3* db, const std::string& username, const double& balance) {
    // Modify the query to order transactions by 'trans_id' in descending order
    const char* select_transactions_sql = "SELECT trans FROM transactions WHERE username = ? ORDER BY id DESC;";
    sqlite3_stmt* stmt;

    // Prepare the SQL query
    if (sqlite3_prepare_v2(db, select_transactions_sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing transaction query: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind the username to the query
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Execute the query and loop through the results
    int row_count = 0;
    std::cout << "Transactions for user: " << username << std::endl;
    std::cout << "Balance: " << std::fixed << std::setprecision(2) << balance << "€\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Retrieve the transaction from the result set
        const unsigned char* transaction = sqlite3_column_text(stmt, 0);
        std::cout << row_count + 1 << ". " << transaction << std::endl;
        row_count++;
    }

    if (row_count == 0) {
        std::cout << "No transactions found for this user." << std::endl;
    }

    // Finalize the statement
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

void insert_user(sqlite3* db, const std::string& username, const std::string& password, double initial_balance) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO users (username, password, balance) VALUES (?, ?, ?);";
    
    // Prepare the SQL query to insert a new user
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    
    // Hash the password before inserting it into the database
    std::string hashedPassword = hashPassword(password);
    
    // Bind the username, hashed password, and initial balance to the SQL statement
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, initial_balance);  // Bind the initial balance
    
    // Execute the query to insert the user
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert user: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);  // Clean up
}

class Account {
	private:
        sqlite3* db;
		std::string username;
		double balance;
	public:
		Account(sqlite3* db, std::string user, std::string password, double user_balance, bool log): username(user), balance(user_balance), db(db) {
			if (log) {
                Transaction initialTransaction = {"Deposit", user_balance, balance, "", getCurrentTimestamp()};
                insert_user(db, user,password,user_balance);
                saveTransaction(db,username,initialTransaction);
            }
		}
        std::string getUsername() {
            return this->username;
        }
		double getBalance() const {
			return balance;
		}
        void deposit(double amount, bool showMessage, Account* recipient = nullptr) {
            double temp_balance;
            std::string name;

            if (recipient == nullptr) {
                temp_balance = balance + amount;  // For current account (this)
                this->balance = temp_balance;    // Update the balance in this object
                name = this->getUsername();
            } else {
                temp_balance = recipient->balance + amount;  // For recipient's account
                recipient->balance = temp_balance;            // Update the recipient's balance
                name = recipient->getUsername();
            }

            // Create the SQL statement to update the balance in the database
            const char* update_sql = "UPDATE users SET balance = ? WHERE username = ?;";
            sqlite3_stmt* stmt;

            // Prepare the SQL statement
            if (sqlite3_prepare_v2(db, update_sql, -1, &stmt, nullptr) != SQLITE_OK) {
                std::cerr << "Error preparing update statement: " << sqlite3_errmsg(db) << std::endl;
                return;
            }

            // Bind the new balance and username to the SQL statement
            sqlite3_bind_double(stmt, 1, temp_balance);  // Bind the new balance
            sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);  // Bind the username

            // Execute the update
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error updating balance: " << sqlite3_errmsg(db) << std::endl;
            } else if (showMessage) {
                std::cout << "Successfully deposited " << amount << "€ to account: " << name << std::endl;
                Transaction deposit = {"Deposit", amount, (recipient == nullptr ? this->balance : recipient->balance), "", getCurrentTimestamp()};
                saveTransaction(db, name, deposit);
            }

            // Finalize the statement to release resources
            sqlite3_finalize(stmt);
        }
        bool withdraw(double amount, bool showMessage) {
            if (amount > 0 && balance >= amount) {
                balance -= amount;

                // Create the SQL statement to update the balance in the database
                const char* update_sql = "UPDATE users SET balance = ? WHERE username = ?;";
                sqlite3_stmt* stmt;

                // Prepare the SQL statement
                if (sqlite3_prepare_v2(db, update_sql, -1, &stmt, nullptr) != SQLITE_OK) {
                    std::cerr << "Error preparing update statement: " << sqlite3_errmsg(db) << std::endl;
                    return false;
                }

                // Bind the new balance and username to the SQL statement
                sqlite3_bind_double(stmt, 1, balance);  // Bind the new balance
                sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);  // Bind the username

                // Execute the update
                if (sqlite3_step(stmt) != SQLITE_DONE) {
                    std::cerr << "Error updating balance: " << sqlite3_errmsg(db) << std::endl;
                    sqlite3_finalize(stmt);
                    return false;
                } else if (showMessage) {
                    std::cout << "Successfully withdrew " << amount << "€ from account: " << this->username << std::endl;
                    Transaction withdrawal = {"Withdrawal", amount, this->balance, "", getCurrentTimestamp()};
                    saveTransaction(db, this->username, withdrawal);
                }

                // Finalize the statement to release resources
                sqlite3_finalize(stmt);
                return true;
            } else {
                std::cout << "Insufficient funds or invalid amount. Withdrawal aborted.\n";
                return false;
            }
        }
		void transfer(double amount, std::unique_ptr<Account>& recipient, sqlite3* db) {
			if(this->withdraw(amount,false)){
				recipient->deposit(amount, false, recipient.get());
				Transaction OutgoingTransferTransaction = {"Outgoing-Transfer", amount, this->balance, recipient->getUsername(), getCurrentTimestamp()};
                std::cin.get();
                saveTransaction(db, this->getUsername(), OutgoingTransferTransaction);
				Transaction incomingTransferTransaction = {"Incoming-Transfer", amount, recipient->balance, this->getUsername(), getCurrentTimestamp()};
                saveTransaction(db, recipient->getUsername(), incomingTransferTransaction);
			}
		}
};

// Function to initialize the database and create the table (if not exists)
int createTables(sqlite3* db) {
    const char* create_users_sql = R"(CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    balance REAL
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);)";

    // Create the 'transactions' table if it doesn't exist
    const char* create_transactions_sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL,
            trans TEXT NOT NULL
        );
    )";

    char* errorMessage;
    
    // Execute the SQL statements
    if (sqlite3_exec(db, create_users_sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        std::cerr << "Error creating users table: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return 1;
    }

    if (sqlite3_exec(db, create_transactions_sql, nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        std::cerr << "Error creating transactions table: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return 1;
    }
    return 0;
}

// Function to find a user by username
bool find_user(sqlite3* db, const std::string& username, std::string& password) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT username, password FROM users WHERE username = ?;";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind the username to the query (index 1 because the first `?` is for username)
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // Execute the query
    int result = sqlite3_step(stmt);
    if (result == SQLITE_ROW) {
        // Retrieve the password hash (index 1 because it's the second column)
        password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return true;  // User found, hashed password is set
    } else {
        sqlite3_finalize(stmt);
        return false; // No such user found
    }
}

// Function to verify the entered password with the hashed password from the database
bool verify_password(const std::string& password, const std::string& hashed_password) {
    // Verify the entered password with the stored hashed password
    return crypto_pwhash_str_verify(hashed_password.c_str(), password.c_str(), password.size()) == 0;
}

bool isValidInput(int &input, int lower_limit, int upper_limit) {
    if (std::cin.fail()) {  // Check if input is not an integer
        std::cin.clear();  // Clear the error exitFlag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
        return false;  // Return false if input is not valid
    }
    if ( input < lower_limit || input > upper_limit ) { // check if input is outside the range the caller requests
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
        return false;
    }
    return true;
}

double getAmount( std::string str) {
    double amount;
    std::cout<<"Please input the amount of money you wish to " << str << ": ";
    std::cin>>amount;
    if (!std::cin.fail() && amount > 0) {  // If input is an integer
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return amount;  
    }
    if (amount<0){
        std::cout<< "Negative amounts are not allowed. Operation aborted";
    } else { std::cout << "Invalid input. Operation aborted\n"; }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return -1;
}

std::unique_ptr<Account> lookupRecipient(sqlite3* db, const std::string& recipientUsername) { 
    sqlite3_stmt* stmt; 
    const char* select_sql = "SELECT * FROM users WHERE username = ?;"; 
    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr) != SQLITE_OK) { 
        std::cerr << "Error preparing query: " << sqlite3_errmsg(db) << std::endl; 
        return nullptr; 
    } 
    sqlite3_bind_text(stmt, 1, recipientUsername.c_str(), -1, SQLITE_STATIC); 
    int result = sqlite3_step(stmt);

    if (result == SQLITE_ROW) {
        std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        double balance = sqlite3_column_double(stmt, 3);
        std::unique_ptr<Account> recipient = std::make_unique<Account>(db, username, password_hash, balance, false);
        sqlite3_finalize(stmt);
        return recipient;
    } else {
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

void clearScreen() {
    std::cout<<std::flush;
    #ifdef _WIN32  // Windows
        system("cls");
    #elif __unix__  // Linux or Unix-like systems
        system("clear");
    #endif
}

void waitForUser() {
    std::cout <<"Press any key to continue...";
    char userInput;
    std::cin.get(userInput);
}

void displayMainMenu(const std::string& username) {
    clearScreen();
    std::cout << "-- Current Account: " << username << " --\n";
    std::cout << "------ Main Menu ------\n";
    std::cout << "1.  View Balance\n";
    std::cout << "2.  Deposit\n";
    std::cout << "3.  Withdraw\n";
    std::cout << "4.  Transfer\n";
    std::cout << "5.  Transaction History\n";
    std::cout << "6.  Create New Account\n";
    std::cout << "7.  Login Into New Account\n";
    std::cout << "8. Log Out And Exit The App\n";
    std::cout << "Choose an option (1-8): ";
}

void handleUserInput(sqlite3* db, std::unique_ptr<Account>& account, bool& flag_finished, int& option_login_or_create) {
    int opt;
    // this while loop is closed when case 10 is run, meaning that the user chose to log out.
    while (true) {
        displayMainMenu(account->getUsername());   
        std::cin>>opt;
        while(!isValidInput(opt,1,8)) {
            displayMainMenu(account->getUsername());
            std::cin>>opt;
        }
        std::cout << std::endl;
        switch (opt) {
            case 1:
                std::cout<< "Your balance is: " << account->getBalance() << "€\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
                break;
            case 2: {
                double amount = getAmount("deposit");
                if(amount>0){
                    account->deposit(amount, true);
                }
                break;
            }
            case 3: {
                double amount = getAmount("withdraw");
                if(amount>0) {
                    account->withdraw(amount,true);
                }
                break;
            }
            case 4: {
                double amount = getAmount("transfer");
                std::string recipientUsername;
                std::cout << "Enter the recipient's username: ";
                std::cin >> recipientUsername;
                std::unique_ptr<Account> recipient = lookupRecipient(db, recipientUsername) ;
                // flag : True  = recipient was found
                // flag : False = recipient was not found
                if ( recipient ){
                    account->transfer(amount, recipient, db);
                } else {
                    std::cout << "Invalid recipient username. Transfer aborted.\n";
                }
                break;
            }
            case 5: 
                printTransactionHistory(db, account->getUsername(), account->getBalance());
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
                break;
            case 6:
                option_login_or_create=2;
                return;
            case 7:
                option_login_or_create=1;
                return;
            case 8:
                flag_finished=true;
                return;
        }
        waitForUser();
	}
}

std::unique_ptr<Account> loginUser(sqlite3* db, const std::string& inputUsername, const std::string& inputPassword) {
    sqlite3_stmt* stmt;
    
    // SQL query to select the username, password, and balance for the given username
    const char* sql = "SELECT username, password, balance FROM users WHERE username = ?;";

    // Prepare the SQL query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    // Bind the input username to the SQL query
    sqlite3_bind_text(stmt, 1, inputUsername.c_str(), -1, SQLITE_STATIC);

    // Execute the query and fetch the result
    int result = sqlite3_step(stmt);
    if (result == SQLITE_ROW) {
        // Retrieve username and password hash from the database
        std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));  // This is the plain text username
        std::string hashedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));  // This is the hashed password
        
        // Retrieve the balance from the database
        double balance = 0.0;
        int balance_type = sqlite3_column_type(stmt, 2);  // Check if balance is null or exists
        if (balance_type != SQLITE_NULL) {
            balance = sqlite3_column_double(stmt, 2);  // Account balance
        }

        // Create and return an Account object with the retrieved data
        std::unique_ptr<Account> loggedInAccount = std::make_unique<Account>(db, username, hashedPassword, balance, false);

        // Finalize the SQL statement
        sqlite3_finalize(stmt);
        return loggedInAccount;
    } else {
        // If no user is found with the given username
        std::cerr << "User not found." << std::endl;
    }

    // Finalize the SQL statement in case of error
    sqlite3_finalize(stmt);
    return nullptr;
}

void create_account(sqlite3* db, bool& finished_flag, int& option_login_or_create) {
    std::string username, password, confirmPassword;
    
    std::cout << "Enter username: ";
    std::cin >> username;
    char option;

    // Check if username already exists
    std::string existingPasswordHash;
    while(find_user(db, username, existingPasswordHash)) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
        std::cout << "Username already exists. Choose another username or press 'q' to quit the account creation process: \n";
        std::cin.get(option);
        if (option=='q'){return;}
        clearScreen();
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
    }

    // Get a valid password and confirm it
    bool pass_flag = false;
    while(!pass_flag) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
        std::cout << "Enter password: ";
        password = getPassword();
        std::cout << "\nConfirm password: ";
        confirmPassword = getPassword();
        if (password != confirmPassword) {
            std::cout << "\nPasswords do not match. Please try again.\n";
            std::cout << "Press any key to continue, or 'q' to quit the process\nYour option: ";
            std::cin.get(option);
            if(option=='q'){return;}
            clearScreen();
        } else { pass_flag = true;}
    }
    double initial_balance = 0.0;
    insert_user(db, username, password, initial_balance);
    std::cout << "\nAccount created successfully!\n";
    //giving user control of his newly created account
    std::unique_ptr<Account> account = loginUser(db, username, password);
    if (account == nullptr ) {
        std::cerr << "\nThe program failed to log you into your new account." << std::endl;
        return;
    }
    waitForUser();
    handleUserInput(db, account, finished_flag, option_login_or_create);
}

// checks if user has surpassed the maximum amount of max_tries 
// this function receives the current try the user is in ( int& tries ), the maximum tries he can use (int& max_tries ) and the duration the lockout should be enforced for
// if he has, the program will ask him to wait for increasing amounts of time (5sec --> 5*3=15seconds --> ...) and will also decrease the max_tries variable by 1 each time, until it reaches its minimum value (which is a single try)
void lockOutLogic( int& tries, int& max_tries, int& duration) {
    if (tries==max_tries+1) {
            for (int i = duration; i>0; --i) {
                clearScreen();
                std::cout << "Too many incorrect attempts, you can try again in: " << i << " seconds" << std::flush;
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

void request_login(sqlite3* db, bool& finished_flag, int& option_login_or_create){
    bool logged_in = false;
    int tries = 1;
    int max_tries=3;
    int duration=5;
    char option{};
    std::string name,pass;
    while(!logged_in) {
        lockOutLogic(tries, max_tries, duration);
        std::cout<<"Enter username: ";
        std::cin>>name;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
        std::cout<<"Enter password: ";
        pass = getPassword();

        // Retrieve the hashed password from the database
        std::string stored_password_hash;
        if (find_user(db, name, stored_password_hash)) {
            // Verify the provided password with the stored hash
            if (verify_password(pass, stored_password_hash)) {
                std::cout << "\nLogged In" << std::endl;
                logged_in=true;
            } else {
                // Invalid password
                std::cout << "Invalid Password or Username. Try again or press 'q' to quit the login process: \n";
                option = std::cin.get();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (option=='q'){return;}
                tries++;
            }
        } else {
            // invalid username
            std::cout << "Invalid Password or Username. Try again or press 'q' to quit the login process: \n";
            option = std::cin.get();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (option=='q'){return;}
            tries++;
        }
    }
    std::unique_ptr<Account> account = loginUser(db, name, pass);
    if (account != nullptr ) {
        handleUserInput(db, account, finished_flag, option_login_or_create);
    } else {
        std::cerr << "\nLogin failed." << std::endl;
    }
}

void requestAuthOptions(sqlite3* db) {
    int option_login_or_create;
    std::cout << "------ Welcome ------\n";
    std::cout << "Would you like to: \n";
    std::cout << "1. Log Into An Account\n";
    std::cout << "2. Create An Account\n";
    std::cout << "Choose an option (1-2): ";
    std::cin>>option_login_or_create;
    while(!isValidInput(option_login_or_create,1,2)) {
        // clear screen command depending on OS, windows will use the system call "cls" and linux "clear"
        clearScreen();
        // display options
        std::cout << "------ Welcome ------\n";
        std::cout << "Would you like to: \n";
        std::cout << "1. Log Into An Account\n";
        std::cout << "2. Create An Account\n";
        std::cout << "Choose an option (1-2): ";
        std::cin>>option_login_or_create;
    }

    bool finished = false;
    while(!finished){
        switch (option_login_or_create) {
            case 1:
                request_login(db, finished, option_login_or_create);
            break;
            case 2:
                create_account(db, finished, option_login_or_create);
            break;
        }
    }
}

// Function to handle login and retrieve account data from SQLite
int main () {
    clearScreen();
	// Initialize libsodium
    if (sodium_init() == -1) {
        std::cerr << "libsodium initialization failed!" << std::endl;
        return 1;
    }

    sqlite3 *db;
    int rc = sqlite3_open("user_data.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    if (createTables(db) != 0) {
        sqlite3_close(db);
        return 1;
    }

    requestAuthOptions(db);
    sqlite3_close(db);
    return 0;
}
