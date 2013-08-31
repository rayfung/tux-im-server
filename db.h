#ifndef DB_H
#define DB_H

#include <QString>

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

private:
    void prepare();
};

#endif // DB_H
