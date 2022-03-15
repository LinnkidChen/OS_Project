#include "FileSystemSimulateWindow.h"
#include "MainWindow.h"
#include "ui_FileSystemSimulateWindow.h"

FileSystemSimulateWindow::FileSystemSimulateWindow(MainWindow *mainWnd,
                                                   QWidget *parent)
    : QWidget(parent),
      ui(std::make_unique<Ui::FileSystemSimulateWindow>()),
      mainWindow(mainWnd) {
  ui->setupUi(this);
  // 默认不显示，从mainWindow调用后显示
  this->hide();

  // 返回mainWindow
  connect(ui->btnBack, &QPushButton::released, this,
          &FileSystemSimulateWindow::SwitchToMainWindow);
}

FileSystemSimulateWindow::~FileSystemSimulateWindow() = default;

void FileSystemSimulateWindow::SwitchToMainWindow() {
  this->hide();
  mainWindow->show();
}
