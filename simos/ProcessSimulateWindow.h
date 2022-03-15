#ifndef PROCESSSIMULATEWINDOW_H
#define PROCESSSIMULATEWINDOW_H

#include <QWidget>
#include <memory>

namespace Ui {
class ProcessSimulateWindow;
}

class MainWindow;

class ProcessSimulateWindow : public QWidget {
  Q_OBJECT

public:
  explicit ProcessSimulateWindow(MainWindow *mainWnd,
                                 QWidget *parent = nullptr);
  ~ProcessSimulateWindow();

private slots:
  void SwitchToMainWindow();

private:
  std::unique_ptr<Ui::ProcessSimulateWindow> ui;
  MainWindow *mainWindow;
};

#endif // PROCESSSIMULATEWINDOW_H
