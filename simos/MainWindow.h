#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class FileSystemSimulateWindow;
class ProcessSimulateWindow;
class MemorySimulateWindow;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void SwitchToFileSystemManagement();
  void SwitchToProcessManagement();
  void SwitchToMemoryManagement();

private:
  std::unique_ptr<Ui::MainWindow> ui;
  std::unique_ptr<FileSystemSimulateWindow> fileSystemSimulation;
  std::unique_ptr<ProcessSimulateWindow> processSimulation;
  std::unique_ptr<MemorySimulateWindow> memorySimulation;
};
#endif // MAINWINDOW_H
