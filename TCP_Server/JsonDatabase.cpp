#include "JsonDatabase.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QDate>
#include <QDir>

void JsonDatabase::Database_Init() {
    QFile file("D:/ITIDA/TCP_Server/database1.json");
    if (!file.exists()) {                //if not existed create new one
        QJsonObject db;
        QJsonArray accounts;
        db["accounts"] = accounts;
        DatabaseWrite(db);
    }
}

void JsonDatabase::DatabaseWrite(const QJsonObject &db) {
    QFile file("D:/ITIDA/TCP_Server/database1.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open database file for writing.";
        return;
    }
    QJsonDocument doc(db);
    file.write(doc.toJson());
    file.close();

}

QJsonObject JsonDatabase::loadDatabase() {
    QFile file("D:/ITIDA/TCP_Server/database1.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open the database file.");
        return {};
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning("Failed to parse the database file.");
        return {};
    }

    return doc.object();
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

    for (const QJsonValue &value : accounts) {
        QJsonObject account = value.toObject();
        if (account["username"].toString() == username) {
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
                tran["date"] = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm");

                QJsonArray transactions = account["transactions"].toArray();
                transactions.append(QJsonValue(tran));
                account["transactions"] = transactions;

                accounts[i] = account;
                db["accounts"] = accounts;

                qDebug() << "Transaction made. New balance: " << account["balance"].toDouble();

                DatabaseWrite(db);

                return account["balance"].toDouble();
            }
        }
    }
    return 0.0;
}


bool JsonDatabase::createUser(const QString &username, const QString &accountNumber, const QString &password, const QString &role, const QString &fullName, int age) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();

    for (const QJsonValue &value : accounts) {
        QJsonObject account = value.toObject();
        if (account["username"].toString() == username) {
            return false;
        }
    }

    QJsonObject newUser;
    newUser["username"] = username;
    newUser["account_number"] = accountNumber;
    newUser["password"] = password;
    newUser["role"] = role;
    newUser["full_name"] = fullName;
    newUser["age"] = age;
    newUser["balance"] = 0;
    newUser["transactions"] = QJsonArray();
    accounts.append(newUser);
    db["accounts"] = accounts;

    DatabaseWrite(db);
    return true;
}
bool JsonDatabase::transferAmount(const QString &fromAccountNumber, const QString &toAccountNumber, double amount, QString &errorMessage) {
    if (amount <= 0) {
        errorMessage = "Transfer amount must be positive";
        return false;
    }

    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();

    QJsonObject fromAccount;
    QJsonObject toAccount;
    int fromIndex = -1;
    int toIndex = -1;

    for (int i = 0; i < accounts.size(); ++i) {
        QJsonObject account = accounts[i].toObject();
        if (account["account_number"].toString() == fromAccountNumber) {
            fromAccount = account;
            fromIndex = i;
        }
        if (account["account_number"].toString() == toAccountNumber) {
            toAccount = account;
            toIndex = i;
        }
    }

    if (fromIndex == -1) {
        errorMessage = "From account number not found";
        return false;
    }

    if (toIndex == -1) {
        errorMessage = "To account number not found";
        return false;
    }

    double fromBalance = fromAccount["balance"].toDouble();
    if (fromBalance < amount) {
        errorMessage = "Insufficient funds";
        return false;
    }

    fromAccount["balance"] = fromBalance - amount;
    toAccount["balance"] = toAccount["balance"].toDouble() + amount;

    QJsonObject fromTransaction;
    fromTransaction["amount"] = -amount;
    fromTransaction["date"] = QDate::currentDate().toString("dd-MM-yyyy hh:mm");
    QJsonArray fromTransactions = fromAccount["transactions"].toArray();
    fromTransactions.append(fromTransaction);
    fromAccount["transactions"] = fromTransactions;

    QJsonObject toTransaction;
    toTransaction["amount"] = amount;
    toTransaction["date"] = QDate::currentDate().toString("dd-MM-yyyy hh:mm");
    QJsonArray toTransactions = toAccount["transactions"].toArray();
    toTransactions.append(toTransaction);
    toAccount["transactions"] = toTransactions;

    accounts[fromIndex] = fromAccount;
    accounts[toIndex] = toAccount;
    db["accounts"] = accounts;
    DatabaseWrite(db);

    return true;
}

bool JsonDatabase::deleteUser(const QString& accountNumber) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();

    for (int i = 0; i < accounts.size(); ++i) {
        QJsonObject account = accounts[i].toObject();
        if (account["account_number"].toString() == accountNumber) {
            accounts.removeAt(i);
            db["accounts"] = accounts;
            DatabaseWrite(db);
            return true;
        }
    }
    return false;
}

bool JsonDatabase::updateUser(const QString &accountNumber, const QString &password, const QString &fullName, int age) {
    QJsonObject db = loadDatabase();
    QJsonArray accounts = db["accounts"].toArray();

    for (int i = 0; i < accounts.size(); ++i) {
        QJsonObject account = accounts[i].toObject();
        if (account["account_number"].toString() == accountNumber) {
            account["password"] = password;
            account["full_name"] = fullName;
            account["age"] = age;
            accounts[i] = account;
            db["accounts"] = accounts;
            DatabaseWrite(db);
            return true;
        }
    }
    return false;
}

