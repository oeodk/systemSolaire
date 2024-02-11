#include "systemSolaire.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    systemSolaire w;
    w.show();
    return a.exec();
}
