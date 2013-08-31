#include "db.h"
#include <QtSql>
#include <QDateTime>

DB::DB()
{
}

bool DB::open(QString dbName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    return db.open();
}

void DB::prepare()
{
    QSqlQuery q;

    q.exec("pragma foreign_keys = ON");
}

bool DB::addUser(QString password, QString nickname, QString gender, QString address,
                 quint32 *id)
{
    prepare();

    QSqlQuery q;
    qsrand(QDateTime::currentDateTime().toTime_t());
    *id = 20000 + qrand() % 102400;
    q.prepare("insert into tbl_user(id, password, nickname, gender, address) "
              "values(?, ?, ?, ?, ?)");
    q.addBindValue(*id);
    q.addBindValue(password);
    q.addBindValue(nickname);
    q.addBindValue(gender);
    q.addBindValue(address);
    return q.exec();
}
