#include "startscreen.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    startScreen s;
    s.show();
    return a.exec();
}
