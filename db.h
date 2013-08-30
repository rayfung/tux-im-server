#ifndef DB_H
#define DB_H

#include <QString>

class DB
{
public:
    DB();
    bool open(QString dbName);
};

#endif // DB_H
