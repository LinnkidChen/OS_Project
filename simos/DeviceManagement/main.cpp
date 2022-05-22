#include "dvmainwindow.h"

#include <QApplication>
//TODO 讨论如何在manager调用ui的代码
DVMainWindow* w;
int main(int argc, char *argv[])
{

  QApplication a(argc, argv);
   DVMainWindow w_;
  w=&w_;
  w_.show();
  return a.exec();
}
