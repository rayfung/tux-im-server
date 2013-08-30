#include <QApplication>
#include <QMessageBox>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server;

    if(!server.listen())
    {
        QMessageBox::critical(0, "Error", "unable to listen");
        return 1;
    }
    return a.exec();
}
