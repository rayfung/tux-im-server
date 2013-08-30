#include "server.h"
#include <QtDebug>

Server::Server(QObject *parent) :
    QObject(parent)
{
}

bool Server::readConfig(QString path)
{
    return config.readConfigFromFile(path);
}

bool Server::listen()
{
    if(!tcpServer.listen(QHostAddress(config.getIP()), config.getPort()))
        return false;
    tcpServer.disconnect();
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    return true;
}

void Server::newConnection()
{
    qDebug() << "newConnection()";
}
