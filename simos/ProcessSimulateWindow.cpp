#include "ProcessSimulateWindow.h"
#include "MainWindow.h"
#include "ui_ProcessSimulateWindow.h"

ProcessSimulateWindow::ProcessSimulateWindow(MainWindow *mainWnd,
                                             QWidget *parent)
    : QWidget(parent),
      ui(std::make_unique<Ui::ProcessSimulateWindow>()),
      mainWindow(mainWnd) {
  ui->setupUi(this);
  this->hide();

  // 返回mainWindow
  connect(ui->btnBack, &QPushButton::released, this,
          &ProcessSimulateWindow::SwitchToMainWindow);
}

ProcessSimulateWindow::~ProcessSimulateWindow() = default;

void ProcessSimulateWindow::SwitchToMainWindow() {
  this->hide();
  mainWindow->show();
}
