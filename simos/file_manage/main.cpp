#include "filewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fileWindow w;
    w.show();
    return a.exec();
}
