#include "server.h"
#include <QDataStream>
#include <QtDebug>
#include <QCryptographicHash>

Server::Server(QObject *parent) :
    QObject(parent)
{
}

bool Server::readConfig(QString path)
{
    return config.readConfigFromFile(path);
}

bool Server::setupDB()
{
    return db.open(config.getDBPath());
}

bool Server::listen()
{
    if(!tcpServer.listen(QHostAddress(config.getIP()), config.getPort()))
        return false;
    tcpServer.disconnect();
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    return true;
}

Connection::Connection()
{
    socket = NULL;
    state = state_recv_request_len;
    account = 0;
    login = false;
}

void Server::newConnection()
{
    qDebug() << "newConnection()";
    QTcpSocket *socket;

    socket = tcpServer.nextPendingConnection();
    if(socket == 0)
        return;
    Connection conn;
    conn.socket = socket;
    conns[socket] = conn;

    connect(socket, SIGNAL(readyRead()), this, SLOT(clientReadyRead()),
            Qt::QueuedConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()),
            Qt::QueuedConnection);
}

void Server::clientReadyRead()
{
    QTcpSocket *socket;

    socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0)
        return;
    if(!conns.contains(socket))
    {
        qWarning() << "Connection not exists";
        return;
    }

    Connection &conn = conns[socket];
    QDataStream in(socket);
    QByteArray response;
    QDataStream out(&response, QIODevice::WriteOnly);

    if(conn.socket != socket)
    {
        qWarning() << "Connection error";
        return;
    }

    in.setVersion(QDataStream::Qt_4_6);
    out.setVersion(QDataStream::Qt_4_6);
    while(1)
    {
        switch(conn.state)
        {
        case Connection::state_recv_request_len:
            if(socket->bytesAvailable() < sizeof(quint32))
                return;
            in >> conn.len;
            conn.state = Connection::state_recv_request_data;
            break;

        case Connection::state_recv_request_data:
            if(socket->bytesAvailable() < conn.len)
                return;
            if(!process(conn, in, out))
            {
                socket->close();
                conn.state = Connection::state_error;
                return;
            }
            sendResponse(socket, response);
            conn.state = Connection::state_recv_request_len;
            break;

        default:
            return;
        }
    }
}

void Server::sendResponse(QTcpSocket *socket, const QByteArray &data)
{
    {
        QDataStream out(socket);

        out.setVersion(QDataStream::Qt_4_6);
        out << (quint32)data.size();
    }
    socket->write(data);
}

bool Server::process(Connection &conn, QDataStream &in, QDataStream &out)
{
    quint8 cmd;

    in >> cmd;
    qDebug() << "cmd: " << cmd;
    if(cmd > 0x02 && !conn.login)
        return false;
    switch(cmd)
    {
    case 0x01:
        return registerAccount(conn, in, out);

    case 0x02:
        return login(conn, in, out);

    case 0x03:
        return getFriendList(conn, in, out);

    case 0x04:
        return getUserProfile(conn, in, out);

    case 0x05:
        return deleteFriend(conn, in, out);

    case 0x06:
        return addFriend(conn, in, out);

    case 0x07:
        return getFriendIPAndPort(conn, in, out);

    case 0xE0:
        return modifyFriendDisplayName(conn, in, out);

    case 0xE1:
        return modifyPassword(conn, in, out);

    case 0xE2:
        return modifyUserProfile(conn, in, out);
    }
    return true;
}

bool Server::registerAccount(Connection &conn, QDataStream &in, QDataStream &out)
{
    QString password, nickname;
    QString gender;
    QString address;
    quint32 accountID;
    bool ok;

    in >> password >> nickname >> gender >> address;
    password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
    ok = db.addUser(password, nickname, gender, address, &accountID);
    if(ok)
        out << accountID;
    qDebug() << "registerAccount: " << ok;
    return ok;
}

bool Server::login(Connection &conn, QDataStream &in, QDataStream &out)
{
    User user;
    QString password;
    QString ip;
    quint16 port;
    bool ok;

    in >> user.id >> password >> ip >> port;
    password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
    ok = db.getUserByID(user);
    if(ok && password == user.password)
    {
        conn.login = true;
        conn.account = user.id;
        conn.ip = ip;
        conn.port = port;
        out << (bool)true << user.nickname << user.gender << user.address;
        qDebug() << "login: " << conn.account << conn.ip << conn.port;
    }
    else
        out << (bool)false;
    return true;
}

Connection *Server::findConnectionByUID(quint32 uid)
{
    QMap<QTcpSocket *, Connection>::iterator iter;

    for(iter = conns.begin(); iter != conns.end(); ++iter)
    {
        if(iter->account == uid)
            return &(*iter);
    }
    return NULL;
}

bool Server::getFriendList(Connection &conn, QDataStream &in, QDataStream &out)
{
    QList<User> friendList;
    QList<QString> displayNameList;
    quint32 num;

    if(!db.getFriendList(conn.account, friendList, displayNameList))
    {
        num = 0;
        out << num;
    }
    else
    {
        num = friendList.size();
        out << num;
        for(quint32 i = 0; i < num; ++i)
        {
            User user = friendList.at(i);
            bool online = false;
            Connection *c;

            c = findConnectionByUID(user.id);
            if(c)
                online = c->login;
            out << user.id << online << user.nickname << displayNameList.at(i);
            out << user.gender << user.address;
        }
    }
    return true;
}

bool Server::getUserProfile(Connection &conn, QDataStream &in, QDataStream &out)
{
    quint32 num;
    User user;

    in >> user.id;
    if(!db.getUserByID(user))
    {
        num = 0;
        out << num;
        return true;
    }

    Connection *c;
    bool online = false;
    c = findConnectionByUID(user.id);
    if(c)
        online = c->login;
    num = 1;
    out << num;
    out << user.id << online << user.nickname;
    out << db.getFriendDisplayName(conn.account, user.id);
    out << user.gender << user.address;
    return true;
}

bool Server::addFriend(Connection &conn, QDataStream &in, QDataStream &out)
{
    quint32 uid;
    QString displayName;

    in >> uid >> displayName;
    out << db.makeFriend(conn.account, uid, displayName);
    return true;
}

bool Server::getFriendIPAndPort(Connection &conn, QDataStream &in, QDataStream &out)
{
    quint32 uid;
    Connection *c;

    in >> uid;
    c = findConnectionByUID(uid);
    if(c)
    {
        out << c->login;
        if(c->login)
            out << c->ip << c->port;
    }
    else
    {
        out << (bool)false;
    }
    return true;
}

bool Server::deleteFriend(Connection &conn, QDataStream &in, QDataStream &out)
{
    quint32 uid;

    in >> uid;
    out << db.removeFriend(conn.account, uid);
    return true;
}

bool Server::modifyFriendDisplayName(Connection &conn, QDataStream &in, QDataStream &out)
{
    quint32 uid;
    QString displayName;

    in >> uid >> displayName;
    out << db.modifyFriendDisplayName(conn.account, uid, displayName);
    return true;
}

bool Server::modifyPassword(Connection &conn, QDataStream &in, QDataStream &out)
{
    QString password;

    in >> password;
    password = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex();
    out << db.modifyPassword(conn.account, password);
    return true;
}

bool Server::modifyUserProfile(Connection &conn, QDataStream &in, QDataStream &out)
{
    User user;

    in >> user.nickname >> user.gender >> user.address;
    user.id = conn.account;
    out << db.modifyUserProfile(user);
    return true;
}

void Server::clientDisconnected()
{
    QTcpSocket *socket;

    socket = qobject_cast<QTcpSocket *>(sender());
    if(socket)
    {
        conns.remove(socket);
        socket->close();
        socket->deleteLater();
        qDebug() << "connection closed";
    }
}
