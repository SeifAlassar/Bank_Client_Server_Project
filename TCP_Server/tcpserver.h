#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QMap>
#include <QThread>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    void startServer(int port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readyRead();
    void disconnected();

private:
    void handleClient(QTcpSocket* socket, const QJsonObject& request, QJsonObject& response);
    QJsonObject readJsonFromSocket(QTcpSocket* socket);
    void sendJsonToSocket(QTcpSocket* socket, const QJsonObject &response);

    QMap<QTcpSocket*, QThread*> clientThreads;
};

#endif // TCPSERVER_H
