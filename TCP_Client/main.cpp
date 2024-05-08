#include <QCoreApplication>
#include <iostream>
#include <QString>
#include <QJsonObject>
#include "TcpClient.h"
#include "qjsonarray.h"

void printUserMenu() {
    std::cout << "Choose an option:\n";
    std::cout << "1. Get Account Number\n";
    std::cout << "2. Get Balance\n";
    std::cout << "3. View Transaction History\n";
    std::cout << "4. Make Transaction\n";
    std::cout << "5. Transfer Amount\n";
    std::cout << "6. Exit\n";
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

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    TcpClient client;

    client.connectToServer("localhost", 54321);

    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

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
            request["type"] = "operation";
            request["operationId"] = choice;
            if (role == "admin") {
                switch(choice) {
                case 1:{
                    QJsonObject request{{"type", "login"}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Account Number: " << response["account_number"].toString().toStdString() << std::endl;
                    } else {
                        std::cout << "Failed to retrieve account number: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }

                case 2:
                {

                        QJsonObject request{{"type", "login"}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                        QJsonObject response = client.sendRequest(request);
                        qDebug() << "Account Balance: " << response["balance"].toDouble();
                        break;

                }


                case 3:
                {
                    QJsonObject request{{"type", "login"}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        auto transactions = response["transactions"].toArray();
                        std::cout << "Transaction History:\n";
                        for (const auto& transaction : transactions) {
                            std::cout << transaction.toString().toStdString() << std::endl;
                        }
                    } else {
                        std::cout << "Failed to retrieve transaction history: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }

                case 4:
                {
                     QJsonObject request{{"type", "login"}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                      QJsonObject response = client.sendRequest(request);

                     if (response["status"].toString() == "success") {
                          QJsonArray accounts =response["accounts"].toArray();
                         foreach (const auto& account,accounts) {
                              qDebug()<<account;
                         }
                     } else {
                         std::cout << "Failed to retrieve database: " << response["error"].toString().toStdString() << std::endl;
                     }
                     break;
                }
                case 5:
                {
                    if (role == "admin") {
                        std::string newUsername, newPassword;
                        std::cout << "Enter new username: ";
                        std::cin >> newUsername;
                        std::cout << "Enter new password: ";
                        std::cin >> newPassword;

                        QJsonObject newUserRequest{
                            {"type", "createUser"},
                            {"username", QString::fromStdString(newUsername)},
                            {"password", QString::fromStdString(newPassword)},
                            {"role", "user"}
                        };

                        QJsonObject createUserResponse = client.sendRequest(newUserRequest);
                        if (createUserResponse["status"].toString() == "success") {
                            std::cout << "New user created successfully.\n";
                        } else {
                            std::cout << "Failed to create user: " << createUserResponse["error"].toString().toStdString() << "\n";
                        }
                    } else {
                        std::cout << "Unauthorized access.\n";
                    }
                    break;
                }
                case 6:
                    long accountnum;
                    std::cout<<"enter account number";
                    std::cin>>accountnum;
                    std::cout << "Deleting user..." << std::endl;
                    break;
                case 7:
                    std::cout<<"enter account number";
                    std::cin>>accountnum;
                    std::cout << "updating user..." << std::endl;;
                    break;
                case 8:
                    std::cout << "Exiting..." << std::endl;
                    return 0;

                default:
                    std::cout << "Invalid Command. Type a number from 1 to 8" << std::endl;
                }
            } else {
                switch(choice) {
                case 1: {
                    QJsonObject request{{"type", "login"}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                    response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        std::cout << "Account Number: " << response["account_number"].toString().toStdString() << std::endl;
                    } else {
                        std::cout << "Failed to retrieve account number: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 2: {
                    QJsonObject request{{"type", "login"}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                    QJsonObject response = client.sendRequest(request);
                    qDebug() << "Account Balance: " << response["balance"].toDouble();
                    break;
                }
                case 3: {
                    QJsonObject request{{"type", "login"},{"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        auto transactions = response["transactions"].toArray();
                        std::cout << "Transaction History:\n";
                        //qDebug()<< response;

                        for (const auto& transaction : transactions) {
                            qDebug()<< transaction;
                        }
                    } else {
                        std::cout << "Failed to retrieve transaction history: " << response["error"].toString().toStdString() << std::endl;
                    }
                    break;
                }
                case 4: {
                    std::cout << "Enter transaction amount: ";
                    double amount;
                    std::cin >> amount;
                    QJsonObject request{{"type", "login"},{"amount",amount}, {"username", QString::fromStdString(username)},{"password", QString::fromStdString(password)}};
                    QJsonObject response = client.sendRequest(request);
                    if (response["status"].toString() == "success") {
                        qDebug() << "Transaction successful. New Balance: " << response["balance"].toDouble();
                    } else {
                        std::cout << "Transaction failed." << std::endl;
                    }
                    break;
                }

                case 5:

                    break;
                case 6:
                    std::cout << "Exiting..." << std::endl;
                    return 0;
                default:
                    std::cout << "Invalid Command. Type a number from 1 to 6" << std::endl;
                }
            }
        }
    } else {
        std::cout << "Login failed: " << response["error"].toString().toStdString() << "\n";
    }

    return app.exec();
}
