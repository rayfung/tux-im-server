#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QString>
#include <QMap>
#include "config.h"
#include "db.h"

class Connection
{
public:
    Connection();

public:
    QTcpSocket *socket;
    enum
    {
        state_recv_request_len, state_recv_request_data,
        state_error
    }state;
    quint32 len;

    quint32 account;
    bool login;
    QString ip;
    quint16 port;
};

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    bool readConfig(QString path);
    bool setupDB();
    bool listen();

private:
    Connection *findConnectionByUID(quint32 uid);
    bool process(Connection &conn, QDataStream &in, QDataStream &out);
    void sendResponse(QTcpSocket *socket, const QByteArray &data);
    bool registerAccount(Connection &conn, QDataStream &in, QDataStream &out);
    bool login(Connection &conn, QDataStream &in, QDataStream &out);
    bool getFriendList(Connection &conn, QDataStream &in, QDataStream &out);
    bool getUserProfile(Connection &conn, QDataStream &in, QDataStream &out);
    bool addFriend(Connection &conn, QDataStream &in, QDataStream &out);
    bool getFriendIPAndPort(Connection &conn, QDataStream &in, QDataStream &out);
    bool deleteFriend(Connection &conn, QDataStream &in, QDataStream &out);
    bool modifyFriendDisplayName(Connection &conn, QDataStream &in, QDataStream &out);
    bool modifyPassword(Connection &conn, QDataStream &in, QDataStream &out);
    bool modifyUserProfile(Connection &conn, QDataStream &in, QDataStream &out);

private:
    Config config;
    QTcpServer tcpServer;
    DB db;
    QMap<QTcpSocket *, Connection> conns;

private slots:
    void newConnection();
    void clientDisconnected();
    void clientReadyRead();
};

#endif // SERVER_H
