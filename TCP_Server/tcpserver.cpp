#include "TcpServer.h"
#include "JsonDatabase.h"
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonDocument>

TcpServer::TcpServer(QObject *parent)
    : QTcpServer(parent) {}

void TcpServer::startServer(int port) {
    if (!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Could not start server on port" << port;
    } else {
        qDebug() << "Server started successfully on port" << port;
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() { this->handleClient(socket); });
        qDebug() << "New connection from" << socket->peerAddress().toString();
    } else {
        delete socket;
    }
}

void TcpServer::handleClient(QTcpSocket* socket) {
    QJsonObject request = readJsonFromSocket(socket);
    QString type = request["type"].toString();
    QJsonObject response;
    qDebug() << "Request type:" << type;

    if (type == "login") {
        QString username = request["username"].toString();
        QString password = request["password"].toString();
        QString role;

        bool authenticated = JsonDatabase::authenticateUser(username, password , role);
        qDebug() << "Username:" << username << "Password:" << password;
        if (authenticated) {
            response["status"] = "success";
            response["role"] = role;
            response["message"] = "Login successful";
            response["account_number"] = JsonDatabase::getAccountNumber(username);
            response["balance"] = JsonDatabase::getAccountBalance(username);

            if(role=="admin")
            {
            QJsonObject Database = JsonDatabase::loadDatabase();
            QJsonArray accounts = Database["accounts"].toArray();
            response["accounts"]=QJsonValue (accounts);
            }

            // QString username = request["username"].toString();
            double amount = request["amount"].toDouble();
            bool result = JsonDatabase::makeTransaction(username, amount);
            if (result) {
                response["status"] = "success";
                response["message"] = "Transaction successful";
                response["balance"] = JsonDatabase::getAccountBalance(username);
            }

            QJsonArray transactions = JsonDatabase::getTransactionHistory(username);
            if (!transactions.isEmpty()) {
                response["status"] = "success";
                response["transactions"] = transactions;
            }



        }

        else {
            response["status"] = "failure";
            response["message"] = "Authentication failed";
        }

        qDebug() << "Response:" << response;
        sendJsonToSocket(socket, response);

        //socket->disconnectFromHost();
    }
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
