#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include "config.h"
#include "db.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    bool readConfig(QString path);
    bool setupDB();
    bool listen();

private:
    Config config;
    QTcpServer tcpServer;
    DB db;

private slots:
    void newConnection();
};

#endif // SERVER_H
