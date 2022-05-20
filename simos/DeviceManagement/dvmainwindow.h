#ifndef DVMAINWINDOW_H
#define DVMAINWINDOW_H
#include<QMessageBox>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
    namespace Ui { class DVMainWindow; }
QT_END_NAMESPACE

    class DVMainWindow : public QMainWindow
{
  Q_OBJECT

      public:
               DVMainWindow(QWidget *parent = nullptr);
  ~DVMainWindow();
      void UpdateContents(DVMainWindow*);
      void addDevice();
      void removeDevice();
     void printTerminal(QString input);
        void printPrinter(QString input);


private:
  Ui::DVMainWindow *ui;
};
#endif // DVMAINWINDOW_H
