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
    QString bindAddress;
    quint16 bindPort;
    QString dbFilePath;
};

#endif // CONFIG_H
