#include "config.h"
#include <QFile>
#include <QTextStream>

Config::Config()
{
    bindAddress = QString("0.0.0.0");
    bindPort    = 9866;
    dbFilePath  = QString("./im.db");
}

QString Config::getIP()
{
    return bindAddress;
}

quint16 Config::getPort()
{
    return bindPort;
}

QString Config::getDBPath()
{
    return dbFilePath;
}

bool Config::readConfigFromFile(QString path)
{
    QFile configFile(path);
    if(!configFile.open(QFile::ReadOnly))
        return false;

    QTextStream in(&configFile);
    QString ip, port, db;

    ip = in.readLine();
    if(ip.isEmpty())
        return false;
    port = in.readLine();
    if(port.isEmpty())
        return false;
    db = in.readLine();
    if(db.isEmpty())
        return false;

    bindAddress = ip;
    bindPort    = port.toUInt();
    dbFilePath  = db;
    return true;
}
