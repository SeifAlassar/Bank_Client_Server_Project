#include <QCoreApplication>
#include <iostream>
#include <QString>
#include <QJsonObject>
#include <QTextStream>
#include "qjsonarray.h"
#include "tcpclient.h"
#include <conio.h>

std::string getPasswordInput() {
    std::string password;
    char ch;
    while ((ch = _getch()) != 13) { // handle enter
        if (ch == 8) { // Handle backspace
            if (!password.empty()) {
                std::cout << "\b \b";
                password.pop_back();
            }
        } else {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}


void printAdminMenu() {
    std::cout << "Choose an option:\n";
    std::cout << "1. Get Account Number\n";
    std::cout << "2. Get Balance\n";
    std::cout << "3. View Transaction History\n";
    std::cout << "4. View Bank Database\n";
    std::cout << "5. Create New User\n";
    std::cout << "6. Delete User\n";
    std::cout << "7. Update User\n";
    std::cout << "8. Exit\n";
}
void printUserMenu() {
    std::cout << "Choose an option:\n";
    std::cout << "1. Get Account Number\n";
    std::cout << "2. Get Balance\n";
    std::cout << "3. View Transaction History\n";
    std::cout << "4. Make Transaction\n";
    std::cout << "5. Transfer Amount\n";
    std::cout << "6. Exit\n";
}
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    TcpClient client;

    client.connectToServer("192.168.1.18", 50001);

    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    password = getPasswordInput();

    QJsonObject loginRequest {
        {"type", "login"},
        {"username", QString::fromStdString(username)},
        {"password", QString::fromStdString(password)}
    };
    QJsonObject response = client.sendRequest(loginRequest);

    if (response["status"].toString() == "success") {
        std::cout << "Login successful!\n";
        QString role = response["role"].toString();

        int choice = 0;
        while (true) {
            if (role == "admin") {
                printAdminMenu();
            } else {
                printUserMenu();
            }

            std::cin >> choice;
            QJsonObject request;

            if (role == "admin") {
                switch (choice) {
                case 1: {
                    request["type"] = "get_account_number";
                    request["username"] = QString::fromStdString(username);
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Account Number: " << response["account_number"].toString().toStdString() << std::endl;
                    } else {
                        std::cout << "Failed to retrieve account number: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 2: {
                    request["type"] = "get_balance";
                    request["username"] = QString::fromStdString(username);
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Account Balance: " << response["balance"].toDouble() << std::endl;
                    } else {
                        std::cout << "Failed to retrieve balance: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 3:    {
                    request["type"] = "get_transaction_history";
                    request["username"] = QString::fromStdString(username);
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        auto transactions = response["transactions"].toArray();
                        std::cout << "Transaction History:\n";
                        for (const auto& transaction : transactions) {
                            QJsonObject tran = transaction.toObject();
                            std::cout << "Amount: " << tran["amount"].toDouble()
                                      << ", Date: " << tran["date"].toString().toStdString() << std::endl;
                        }
                    } else {
                        std::cout << "Failed to retrieve transaction history: " << response["message"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 4: {
                    request["type"] = "get_database";
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        QJsonArray accounts = response["accounts"].toArray();
                        std::cout << "Database:\n";
                        for (const auto& account : accounts) {
                            QJsonObject acc = account.toObject();
                            std::cout << "Username: " << acc["username"].toString().toStdString()
                                      << ", Account Number: " << acc["account_number"].toString().toStdString()
                                      << ", Balance: " << acc["balance"].toDouble()
                                      << ", Full Name: " << acc["full_name"].toString().toStdString()
                                      << ", Age: " << acc["age"].toInt() << std::endl;
                        }
                    } else {
                        std::cout << "Failed to retrieve database: " << response["message"].toString().toStdString() << std::endl;
                    }
                    break;
                }


                case 5:  {
                    std::string newUsername, newPassword, accountNumber, fullName;
                    int age;
                    std::cout << "Enter new username: ";
                    std::cin >> newUsername;
                    std::cout << "Enter new password: ";
                    std::cin >> newPassword;
                    std::cout << "Enter new account number: ";
                    std::cin >> accountNumber;
                    std::cout << "Enter full name: ";
                    std::cin.ignore();                                 // To clear the newline character from the input buffer
                    std::getline(std::cin, fullName);
                    std::cout << "Enter age: ";
                    std::cin >> age;
                    client.createNewUser(QString::fromStdString(newUsername), QString::fromStdString(accountNumber), QString::fromStdString(newPassword), QString::fromStdString(fullName), age);
                    break;
                }
                case 6: {
                    std::string accountNumber;
                    std::cout << "Enter account number to delete: ";
                    std::cin >> accountNumber;
                    request["type"] = "delete_user";
                    request["account_number"] = QString::fromStdString(accountNumber);
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "User deleted successfully.\n";
                    } else {
                        std::cout << "Failed to delete user: " << response["message"].toString().toStdString() << "\n";
                    }
                    break;
                }
                case 7: {
                    std::string accountNumber, password, fullName;
                    int age;
                    std::cout << "Enter account number: ";
                    std::cin >> accountNumber;
                    std::cout << "Enter new password: ";
                    std::cin >> password;
                    std::cout << "Enter full name: ";
                    std::cin.ignore();
                    std::getline(std::cin, fullName);
                    std::cout << "Enter age: ";
                    std::cin >> age;

                    request["type"] = "update_user";
                    request["account_number"] = QString::fromStdString(accountNumber);
                    request["password"] = QString::fromStdString(password);
                    request["full_name"] = QString::fromStdString(fullName);
                    request["age"] = age;

                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "User updated successfully!" << std::endl;
                    } else {
                        std::cout << "Failed to update user: " << response["message"].toString().toStdString() << std::endl;
                    }
                    break;
                }

                case 8:
                    std::cout << "Exiting..." << std::endl;
                    return 0;

                default:
                    std::cout << "Invalid Command. Type a number from 1 to 6" << std::endl;
                }
            } else {
                switch (choice) {
                case 1: {
                    request["type"] = "get_account_number";
                    request["username"] = QString::fromStdString(username);
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Account Number: " << response["account_number"].toString().toStdString() << std::endl;
                    } else {
                        std::cout << "Failed to retrieve account number: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 2: {
                    request["type"] = "get_balance";
                    request["username"] = QString::fromStdString(username);
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Account Balance: " << response["balance"].toDouble() << std::endl;
                    } else {
                        std::cout << "Failed to retrieve balance: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 3:   {
                    request["type"] = "get_transaction_history";
                    request["username"] = QString::fromStdString(username);
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        auto transactions = response["transactions"].toArray();
                        std::cout << "Transaction History:\n";
                        for (const auto& transaction : transactions) {
                            QJsonObject tran = transaction.toObject();
                            std::cout << "Amount: " << tran["amount"].toDouble()
                                      << ", Date: " << tran["date"].toString().toStdString() << std::endl;
                        }
                    } else {
                        std::cout << "Failed to retrieve transaction history: " << response["message"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 4: {
                    std::cout << "Enter transaction amount: ";
                    double amount;
                    std::cin >> amount;
                    request["type"] = "make_transaction";
                    request["username"] = QString::fromStdString(username);
                    request["amount"] = amount;
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Transaction successful. New Balance: " << response["balance"].toDouble() << std::endl;
                    } else {
                        std::cout << "Transaction failed: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 5: {
                    std::string fromAccountNumber, toAccountNumber;
                    double amount;
                    std::cout << "Enter from account number: ";
                    std::cin >> fromAccountNumber;
                    std::cout << "Enter to account number: ";
                    std::cin >> toAccountNumber;
                    std::cout << "Enter amount to transfer: ";
                    std::cin >> amount;
                    request["type"] = "transfer_amount";
                    request["from_account_number"] = QString::fromStdString(fromAccountNumber);
                    request["to_account_number"] = QString::fromStdString(toAccountNumber);
                    request["amount"] = amount;
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Transfer successful.\n";
                    } else {
                        std::cout << "Transfer failed: " << response["message"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 6:
                    std::cout << "Exiting..."<< "Thanks for using ITIDA Bank" << std::endl;
                    return 0;
                default:
                    std::cout << "Invalid Command. Type a number from 1 to 5" << std::endl;
                }
            }
        }
    } else {
        std::cout << "Login failed: " << response["error"].toString().toStdString() << std::endl;
    }

    return app.exec();
}
