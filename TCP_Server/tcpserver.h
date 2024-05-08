#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

class TcpServer : public QTcpServer {
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    void startServer(int port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void handleClient(QTcpSocket* socket);

private:
    QJsonObject readJsonFromSocket(QTcpSocket* socket);
    void sendJsonToSocket(QTcpSocket* socket, const QJsonObject &response);
};

#endif // TCPSERVER_H
