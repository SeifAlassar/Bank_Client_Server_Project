#include <QCoreApplication>
#include "tcpserver.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    TcpServer server;

    if (!server.listen(QHostAddress::Any, 50001)) {
        qDebug() << "Failed to start the server";
        return -1;
    }
    qDebug() << "Server started successfully on port 50001";

    return app.exec();
}
