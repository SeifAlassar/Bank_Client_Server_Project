#include "JsonDatabase.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>


void JsonDatabase::initializeDatabase() {
    QFile file("database.json");
    if (!file.exists()) {
        QJsonObject db;
        QJsonArray accounts;
        db["accounts"] = accounts;
        saveDatabase(db);
    }
}

QJsonObject JsonDatabase::loadDatabase() {
    QFile file("database.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open the database file.");
        return {};
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning("Failed to parse the database file.");
        return {};
    }

    return doc.object();
}
void JsonDatabase::saveDatabase(const QJsonObject &db) {
    QFile file("database.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open database file for writing.";
        return;
    }
    QJsonDocument doc(db);
    file.write(doc.toJson());
    file.close();
}

bool JsonDatabase::authenticateUser(const QString &username, const QString &password, QString& role) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();

    for (const QJsonValue &value : accounts) {
        QJsonObject account = value.toObject();
        if (account["username"].toString() == username && account["password"].toString() == password) {
            role = account["role"].toString();
            return true;
        }
    }
    return false;
}

QString JsonDatabase::getAccountNumber(const QString& username) {
    QJsonObject database = loadDatabase();
    QJsonArray accounts = database["accounts"].toArray();

    foreach (const QJsonValue& value, accounts) {
        QJsonObject account = value.toObject();
        qDebug()<<account;
        if (account["username"].toString() == username) {
            qDebug()<<account["account_number"].toString();
            return account["account_number"].toString();
        }
    }
    return "";
}

double JsonDatabase::getAccountBalance(const QString &username) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();

    for (const QJsonValue &value : accounts) {
        QJsonObject account = value.toObject();
        if (account["username"].toString() == username) {
            return account["balance"].toDouble();
        }
    }
    return 0.0;
}

QJsonArray JsonDatabase::getTransactionHistory(const QString &username) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();
    for (const QJsonValue &value : accounts) {
        QJsonObject account = value.toObject();
        if (account["username"].toString() == username) {
            return account["transactions"].toArray();
        }
    }
    return QJsonArray();
}

double JsonDatabase::makeTransaction(const QString& username, double amount) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();
    for (int i = 0; i < accounts.size(); ++i) {
        QJsonObject account = accounts[i].toObject();
        if (account["username"].toString() == username) {
            double currentBalance = account["balance"].toDouble();
            qDebug() << "current balance: " << currentBalance;

            if (amount > 0) {
                account["balance"] = currentBalance + amount;
                accounts[i] = account;
                db["accounts"] = accounts;

                QJsonObject tran;
                tran["amount"] = amount;
                tran["date"] = QDate::currentDate().toString("dd-MM-yyyy");


                QJsonArray transactions = account["transactions"].toArray();


                transactions.append(QJsonValue(tran));


                account["transactions"] = transactions;
                accounts[i] = account;
                db["accounts"] = accounts;

                qDebug() << "Transaction made. New balance: " << account["balance"].toDouble();


                saveDatabase(db);

                return account["balance"].toDouble();
            }

        }
    }
    return 0.0;
    }

// bool JsonDatabase::createUser(const QString& username, const QString& password, const QString& fullName, int age, double balance) {
//     QJsonObject db = loadDatabase();
//     QJsonArray accounts = db["accounts"].toArray();

//     QJsonObject newUser;
//     newUser["username"] = username;
//     newUser["password"] = password;
//     newUser["fullName"] = fullName;
//     newUser["age"] = age;
//     newUser["balance"] = balance;


//     for (auto value : accounts) {
//         if (value.toObject()["username"].toString() == username) {
//             return false;
//         }
//     }

//     accounts.append(newUser);
//     db["accounts"] = accounts;
//     saveDatabase(db);
//     return true;
// }

