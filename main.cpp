#include "mainscence.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Mainscence w;
    w.show();

    return a.exec();
}
