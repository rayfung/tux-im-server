#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config
{
public:
    Config();
    bool readConfigFromFile(QString path);
    QString getIP();
    quint16 getPort();
    QString getDBPath();

private:
    QString bindAddress; //监听IP地址
    quint16 bindPort; //监听端口号
    QString dbFilePath; //数据库文件路径
};

#endif // CONFIG_H
