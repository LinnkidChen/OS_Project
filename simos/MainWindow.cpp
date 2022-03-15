#include "MainWindow.h"
#include "FileSystemSimulateWindow.h"
#include "MemorySimulateWindow.h"
#include "ProcessSimulateWindow.h"
#include "ui_MainWindow.h"

#include <cstdlib>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(std::make_unique<Ui::MainWindow>()),
      fileSystemSimulation(std::make_unique<FileSystemSimulateWindow>(this)),
      processSimulation(std::make_unique<ProcessSimulateWindow>(this)),
      memorySimulation(std::make_unique<MemorySimulateWindow>(this)) {
  ui->setupUi(this);

  // 跳转到进程模拟
  connect(ui->btnProcessManagement, &QPushButton::released, this,
          &MainWindow::SwitchToProcessManagement);

  // 跳转到文件系统模拟
  connect(ui->btnFileSystemManagement, &QPushButton::released, this,
          &MainWindow::SwitchToFileSystemManagement);

  // 跳转到内存模拟
  // TODO: Implement this
  connect(ui->btnMemoryManagement, &QPushButton::released, this,
          &MainWindow::SwitchToMemoryManagement);

  connect(ui->btnExit, &QPushButton::released, []() { std::exit(0); });
}

MainWindow::~MainWindow() = default;

void MainWindow::SwitchToProcessManagement() {
  this->hide();
  processSimulation->show();
}

void MainWindow::SwitchToFileSystemManagement() {
  this->hide();
  fileSystemSimulation->show();
}

void MainWindow::SwitchToMemoryManagement() {
  this->hide();
  memorySimulation->show();
}
