#include <QApplication>
#include <QMessageBox>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server;

    server.readConfig("./im.cfg");
    if(!server.setupDB())
    {
        QMessageBox::critical(0, "Error", "unable to connect to database");
        return 1;
    }
    if(!server.listen())
    {
        QMessageBox::critical(0, "Error", "unable to listen");
        return 1;
    }
    return a.exec();
}
