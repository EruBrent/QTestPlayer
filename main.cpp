#include "QTestPlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTestPlayer w;
    w.show();
    return a.exec();
}
