#include "tcpclient.h"
#include <QDebug>
#include <iostream>

TcpClient::TcpClient(QObject *parent) : QObject(parent), tcpSocket(new QTcpSocket(this)) {}

void TcpClient::connectToServer(const QString &host, int port) {
    tcpSocket->connectToHost(host, port);
    if (!tcpSocket->waitForConnected(5000)) {
        qDebug() << "Failed to connect to server:" << tcpSocket->errorString();
    } else {
        qDebug() << "Connected to server";
    }
}


bool TcpClient::login(const QString &username, const QString &password) {
    QJsonObject request;
    request["type"] = "login";
    request["username"] = username;
    request["password"] = password;
    QJsonObject response = sendRequest(request);
    return response["status"].toString() == "success";
}

QJsonObject TcpClient::sendRequest(const QJsonObject &request) {
    QJsonDocument doc(request);
    tcpSocket->write(doc.toJson());
    tcpSocket->waitForBytesWritten();
    tcpSocket->waitForReadyRead(5000);
    QJsonDocument responseDoc = QJsonDocument::fromJson(tcpSocket->readAll());
    return responseDoc.object();
}

void TcpClient::createNewUser(const QString &username, const QString &account_number, const QString &password, const QString &fullName, int age) {
    QJsonObject newUser;
    newUser["type"] = "create_user";
    newUser["username"] = username;
    newUser["account_number"] = account_number;
    newUser["password"] = password;
    newUser["role"] = "user";
    newUser["full_name"] = fullName;
    newUser["age"] = age;

    QJsonObject response = sendRequest(newUser);

    if (response["status"].toString() == "success") {
        qDebug() << "User created successfully";
    } else {
        qDebug() << "Failed to create user: " << response["message"].toString();
    }
}

