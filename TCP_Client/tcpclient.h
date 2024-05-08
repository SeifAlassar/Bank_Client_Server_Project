#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

class TcpClient : public QObject {
    Q_OBJECT
public:
    TcpClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, int port);
    bool login(const QString &username, const QString &password);
    QJsonObject sendRequest(const QJsonObject &request);
    QJsonObject loadDatabase();
    QString getAccountNumber(const QString& username);
    double getBalance(const QString& username);
    void createNewUser(const QString &username, const QString &account_number, const QString &password);




private:
    QTcpSocket *tcpSocket;

    QString databasePath;

};

#endif // TCPCLIENT_H
