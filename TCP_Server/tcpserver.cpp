#include "tcpserver.h"
#include "JsonDatabase.h"
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonDocument>

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent) {}

void TcpServer::startServer(int port) {             //takes port number and print in server terminal if server is started or not and the port number
    if (!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Could not start server on port" << port;
    } else {
        qDebug() << "Server started successfully on port" << port;
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);                                  //create new Qtcp object
    if (socket->setSocketDescriptor(socketDescriptor)) {                         //bind the tcpsocket to the socketdes connection
        connect(socket, &QTcpSocket::readyRead, this, &TcpServer::readyRead);       //if binding is successful connect the signals
        connect(socket, &QTcpSocket::disconnected, this, &TcpServer::disconnected);
        qDebug() << "New connection from" << socket->peerAddress().toString();
    } else {
        delete socket;                                                              //delete socket if fails
    }
}

void TcpServer::readyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        qDebug() << "Invalid socket in readyRead";
        return;
    }

    QByteArray jsonData = socket->readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);   //parsing the json data
    QJsonObject request = document.object();                     //extracting the json object from json document

    QJsonObject response;
    QString type = request["type"].toString();

    handleClient(socket, request, response);
}

void TcpServer::handleClient(QTcpSocket* socket, const QJsonObject& request, QJsonObject& response)
{
    QString type = request["type"].toString();
    if (type == "login") {
        QString username = request["username"].toString();
        QString password = request["password"].toString();
        QString role;

        bool authenticated = JsonDatabase::authenticateUser(username, password, role);
        qDebug() << "Username:" << username << "Password:" << password;
        if (authenticated) {
            response["status"] = "success";
            response["role"] = role;
            response["message"] = "Login successful";
            response["account_number"] = JsonDatabase::getAccountNumber(username);
            response["balance"] = JsonDatabase::getAccountBalance(username);

            if (role == "admin") {
                QJsonObject Database = JsonDatabase::loadDatabase();
                QJsonArray accounts = Database["accounts"].toArray();
                response["accounts"] = QJsonValue(accounts);
            }
        } else {
            response["status"] = "failure";
            response["message"] = "Authentication failed";
        }

        sendJsonToSocket(socket, response);

    } else if (type == "create_user") {
        QString username = request["username"].toString();
        QString accountNumber = request["account_number"].toString();
        QString password = request["password"].toString();
        QString role = request["role"].toString();
        QString fullName = request["full_name"].toString();
        int age = request["age"].toInt();

        bool created = JsonDatabase::createUser(username, accountNumber, password, role, fullName, age);
        if (created) {
            response["status"] = "success";
            response["message"] = "User created successfully";
        } else {
            response["status"] = "failure";
            response["message"] = "User already exists";
        }

        sendJsonToSocket(socket, response);

    }
    else if (type == "delete_user") {
        QString accountNumber = request["account_number"].toString();

        bool deleted = JsonDatabase::deleteUser(accountNumber);
        if (deleted) {
            response["status"] = "success";
            response["message"] = "User deleted successfully";
        } else {
            response["status"] = "failure";
            response["message"] = "User not found";
        }
        sendJsonToSocket(socket, response);
    }
    else if (type == "update_user") {
        QString accountNumber = request["account_number"].toString();
        QString password = request["password"].toString();
        QString fullName = request["full_name"].toString();
        int age = request["age"].toInt();

        bool updated = JsonDatabase::updateUser(accountNumber, password, fullName, age);
        if (updated) {
            response["status"] = "success";
            response["message"] = "User updated successfully";
        } else {
            response["status"] = "failure";
            response["message"] = "User not found";
        }
        sendJsonToSocket(socket, response);
    }

    else if (type == "get_balance") {
        QString username = request["username"].toString();
        double balance = JsonDatabase::getAccountBalance(username);
        response["status"] = "success";
        response["balance"] = balance;
        sendJsonToSocket(socket, response);
    } else if (type == "get_account_number") {
        QString username = request["username"].toString();
        QString accountNumber = JsonDatabase::getAccountNumber(username);
        response["status"] = "success";
        response["account_number"] = accountNumber;
        sendJsonToSocket(socket, response);
    }  else if (type == "get_transaction_history") {
        QString username = request["username"].toString();
        QJsonArray transactions = JsonDatabase::getTransactionHistory(username);
        response["status"] = "success";
        response["transactions"] = transactions;
        sendJsonToSocket(socket, response);

    }else if (type == "get_database") {
        QJsonObject Database = JsonDatabase::loadDatabase();
        QJsonArray accounts = Database["accounts"].toArray();
        response["status"] = "success";
        response["accounts"] = accounts;
        sendJsonToSocket(socket, response);

    } else if (type == "make_transaction") {
        QString username = request["username"].toString();
        double amount = request["amount"].toDouble();
        double newBalance = JsonDatabase::makeTransaction(username, amount);
        response["status"] = "success";
        response["balance"] = newBalance;
        sendJsonToSocket(socket, response);
    }
    else if (type == "transfer_amount") {
        QString fromAccountNumber = request["from_account_number"].toString();
        QString toAccountNumber = request["to_account_number"].toString();
        double amount = request["amount"].toDouble();
        QString errorMessage;

        bool success = JsonDatabase::transferAmount(fromAccountNumber, toAccountNumber, amount, errorMessage);
        if (success) {
            response["status"] = "success";
            response["message"] = "Transfer successful";
            response["transactions"] = JsonDatabase::getTransactionHistory(request["username"].toString());
        } else {
            response["status"] = "failure";
            response["message"] = errorMessage;
        }

        sendJsonToSocket(socket, response);
    }
}

void TcpServer::disconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        qDebug() << "Invalid socket in disconnected";
        return;
    }

    qDebug() << "Client disconnected" << socket;
    socket->deleteLater();
}

QJsonObject TcpServer::readJsonFromSocket(QTcpSocket* socket) {
    QByteArray jsonData = socket->readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    return document.object();
}

void TcpServer::sendJsonToSocket(QTcpSocket* socket, const QJsonObject &response) {
    QJsonDocument document(response);
    socket->write(document.toJson());
    socket->waitForBytesWritten();
}
