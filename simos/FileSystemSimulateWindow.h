#ifndef FILESYSTEMSIMULATEWINDOW_H
#define FILESYSTEMSIMULATEWINDOW_H

#include <QWidget>
#include <memory>

namespace Ui {
class FileSystemSimulateWindow;
}

class MainWindow;

class FileSystemSimulateWindow : public QWidget {
  Q_OBJECT

public:
  explicit FileSystemSimulateWindow(MainWindow *mainWnd,
                                    QWidget *parent = nullptr);
  ~FileSystemSimulateWindow();

private slots:
  void SwitchToMainWindow();

private:
  std::unique_ptr<Ui::FileSystemSimulateWindow> ui;
  MainWindow *mainWindow;
};

#endif // FILESYSTEMSIMULATEWINDOW_H
