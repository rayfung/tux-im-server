#include "db.h"
#include <QtSql>
#include <QDateTime>

DB::DB()
{
}

/* 打开数据库 */
bool DB::open(QString dbName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    return db.open();
}

/* 操作数据库之前的准备工作 */
void DB::prepare()
{
    QSqlQuery q;

    q.exec("pragma foreign_keys = ON");
}

/* 添加一个用户，并返回该用户的标识符 */
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

/* 根据用户的标识符获取用户信息 */
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

/* 获取某个用户的好友列表（及备注名称） */
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

/* 获取某个用户的某个好友的备注名称 */
QString DB::getFriendDisplayName(quint32 uid, quint32 friendID)
{
    prepare();

    QSqlQuery q;

    q.prepare("select display_name from tbl_friend "
              "where user_id = ? and friend_id = ?");
    q.addBindValue(uid);
    q.addBindValue(friendID);
    if(!q.exec())
        return QString();
    if(!q.next())
        return QString();
    return q.value(0).toString();
}

/* 添加某个用户为好友，并设置备注名称 */
bool DB::makeFriend(quint32 uid, quint32 friendID, QString displayName)
{
    prepare();

    QSqlQuery q;

    q.prepare("insert into tbl_friend(user_id, friend_id, display_name) "
              "values(?, ?, ?)");
    q.addBindValue(uid);
    q.addBindValue(friendID);
    q.addBindValue(displayName);
    return q.exec();
}

/* 删除好友 */
bool DB::removeFriend(quint32 uid, quint32 friendID)
{
    prepare();

    QSqlQuery q;

    q.prepare("delete from tbl_friend where user_id = ? and friend_id = ?");
    q.addBindValue(uid);
    q.addBindValue(friendID);
    return q.exec();
}

/* 修改好友备注名称 */
bool DB::modifyFriendDisplayName(quint32 uid, quint32 friendID, QString displayName)
{
    prepare();

    QSqlQuery q;

    q.prepare("update tbl_friend set display_name = ? "
              "where user_id = ? and friend_id = ?");
    q.addBindValue(displayName);
    q.addBindValue(uid);
    q.addBindValue(friendID);
    return q.exec();
}

/* 修改用户密码，传入的密码应该已经经过SHA-1哈希处理 */
bool DB::modifyPassword(quint32 uid, QString password)
{
    prepare();

    QSqlQuery q;

    q.prepare("update tbl_user set password = ? where id = ?");
    q.addBindValue(password);
    q.addBindValue(uid);
    return q.exec();
}

/* 修改用户信息 */
bool DB::modifyUserProfile(User user)
{
    prepare();

    QSqlQuery q;

    q.prepare("update tbl_user set nickname = ?, gender = ?, address = ? "
              "where id = ?");
    q.addBindValue(user.nickname);
    q.addBindValue(user.gender);
    q.addBindValue(user.address);
    q.addBindValue(user.id);
    return q.exec();
}
