#ifndef JSONDATABASE_H
#define JSONDATABASE_H

#include <QJsonObject>
#include <QString>
#include <QJsonArray>

class JsonDatabase {
public:
    static void Database_Init();
    static QJsonObject loadDatabase();
    static void DatabaseWrite(const QJsonObject &db);
    static bool authenticateUser(const QString &username, const QString &password, QString& role);
    static QString getAccountNumber(const QString &username);
    static double getAccountBalance(const QString &username);
    static double makeTransaction(const QString& username, double amount);
    static QJsonArray getTransactionHistory(const QString &username);
    static bool transferAmount(const QString &fromAccountNumber, const QString &toAccountNumber, double amount, QString &errorMessage);
    static bool deleteUser(const QString& accountNumber);
    static bool updateUser(const QString &accountNumber, const QString &password, const QString &fullName, int age);
    static bool createUser(const QString &username, const QString &accountNumber, const QString &password, const QString &role, const QString &fullName, int age);
};

#endif // JSONDATABASE_H
