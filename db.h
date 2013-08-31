#ifndef DB_H
#define DB_H

#include <QString>
#include <QList>

class User
{
public:
    quint32 id;
    QString password;
    QString nickname;
    QString gender;
    QString address;
};

class DB
{
public:
    DB();
    bool open(QString dbName);
    bool addUser(QString password, QString nickname, QString gender, QString address,
                 quint32 *id);
    bool getUserByID(User &user);
    bool getFriendList(quint32 id, QList<User> &userList, QList<QString> &displayNameList);
    QString getFriendDisplayName(quint32 uid, quint32 friendID);
    bool makeFriend(quint32 uid, quint32 friendID, QString displayName);
    bool removeFriend(quint32 uid, quint32 friendID);
    bool modifyFriendDisplayName(quint32 uid, quint32 friendID, QString displayName);
    bool modifyPassword(quint32 uid, QString password);
    bool modifyUserProfile(User user);

private:
    void prepare();
};

#endif // DB_H
