#include "MemorySimulateWindow.h"
#include "MainWindow.h"
#include "ui_MemorySimulateWindow.h"

MemorySimulateWindow::MemorySimulateWindow(MainWindow *mainWnd, QWidget *parent)
    : QWidget(parent), ui(new Ui::MemorySimulateWindow), mainWindow(mainWnd) {
  ui->setupUi(this);
  this->hide();

  // 返回mainWindow
  connect(ui->btnBack, &QPushButton::released, this,
          &MemorySimulateWindow::SwitchToMainWindow);
}

MemorySimulateWindow::~MemorySimulateWindow() = default;

void MemorySimulateWindow::SwitchToMainWindow() {
  this->hide();
  mainWindow->show();
}
