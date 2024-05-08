#include <QCoreApplication>
#include "tcpserver.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    TcpServer server;

    if (!server.listen(QHostAddress::LocalHost, 54321)) {
        qDebug() << "Failed to start the server";
        return -1;
    }
    qDebug() << "Server started successfully on port 5000";

    return app.exec();
}
