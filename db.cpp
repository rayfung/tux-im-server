#include "db.h"
#include <QtSql>

DB::DB()
{
}

bool DB::open(QString dbName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    return db.open();
}
