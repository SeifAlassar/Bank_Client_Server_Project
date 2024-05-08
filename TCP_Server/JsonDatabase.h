#ifndef JSONDATABASE_H
#define JSONDATABASE_H

#include "qjsonarray.h"
#include <QJsonObject>
#include <QString>

class JsonDatabase {
public:
    static void initializeDatabase();
     static QJsonObject loadDatabase();
    static void saveDatabase(const QJsonObject &db);

     static bool authenticateUser(const QString &username, const QString &password, QString& role);
    static QString getAccountNumber(const QString &username);
    static double getAccountBalance(const QString &username);
    static double makeTransaction(const QString& username, double amount);
    static QJsonArray getTransactionHistory(const QString &username);



};

#endif // JSONDATABASE_H
