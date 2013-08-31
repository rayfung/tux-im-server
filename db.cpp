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

bool DB::getUserByID(User &user)
{
    prepare();

    QSqlQuery q;

    q.prepare("select password, nickname, gender, address from tbl_user "
              "where id=?");
    q.addBindValue(user.id);
    if(!q.exec())
        return false;
    if(!q.next())
        return false;
    user.password = q.value(0).toString();
    user.nickname = q.value(1).toString();
    user.gender   = q.value(2).toString();
    user.address  = q.value(3).toString();
    return true;
}
