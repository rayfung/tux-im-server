#ifndef DB_H
#define DB_H

#include <QString>

class DB
{
public:
    DB();
    bool open(QString dbName);
    bool addUser(QString password, QString nickname, QString gender, QString address,
                 quint32 *id);

private:
    void prepare();
};

#endif // DB_H
