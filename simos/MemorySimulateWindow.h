#ifndef MEMORYSIMULATEWINDOW_H
#define MEMORYSIMULATEWINDOW_H

#include <QWidget>

namespace Ui {
class MemorySimulateWindow;
}

class MainWindow;

class MemorySimulateWindow : public QWidget {
  Q_OBJECT

public:
  explicit MemorySimulateWindow(MainWindow *mainWnd, QWidget *parent = nullptr);
  ~MemorySimulateWindow();

private slots:
  void SwitchToMainWindow();

private:
  std::unique_ptr<Ui::MemorySimulateWindow> ui;
  MainWindow *mainWindow;
};

#endif // MEMORYSIMULATEWINDOW_H
