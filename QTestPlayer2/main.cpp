#include "QTestPlayer2.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTestPlayer2 w;
    w.show();
    return a.exec();
}
