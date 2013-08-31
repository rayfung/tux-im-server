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

bool DB::getFriendList(quint32 id, QList<User> &userList, QList<QString> &displayNameList)
{
    prepare();

    QSqlQuery q;

    q.prepare("select tbl_user.id, tbl_user.password, tbl_user.nickname, "
              "tbl_user.gender, tbl_user.address, tbl_friend.display_name "
              "from tbl_user, tbl_friend "
              "where tbl_friend.user_id = ? and tbl_friend.friend_id = tbl_user.id");
    q.addBindValue(id);
    if(!q.exec())
        return false;
    while(q.next())
    {
        User user;

        user.id = q.value(0).toUInt();
        user.password = q.value(1).toString();
        user.nickname = q.value(2).toString();
        user.gender = q.value(3).toString();
        user.address = q.value(4).toString();
        userList.append(user);
        displayNameList.append(q.value(5).toString());
    }
    return true;
}
