#include "dvmainwindow.h"
#include "ui_dvmainwindow.h"
#include "DeviceManager.cpp"

DVMainWindow::DVMainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::DVMainWindow)
{
  ui->setupUi(this);
  this->setWindowTitle("设备管理");
  ui->label->setText("类型");

  ui->label_2->setText("设备ID");
  ui->label_3->setText("设备名称");
  ui->label_4->setText("状态");
  ui->label_5->setText("选择添加设备的类型");
  ui->TypeChoose->addItem(QString("Keyboard"));
  ui->TypeChoose->addItem(QString("Disk"));
  ui->TypeChoose->addItem(QString("Printer"));
  ui->TypeChoose->addItem(QString("terminal"));
  ui->add->setText("添加");
  ui->delete_->setText("删除");
  ui->refresh->setText("刷新");

  DeviceManager::addDevice(DeviceDict::disk);
  connect(ui->refresh,&QPushButton::clicked,[this](){this->UpdateContents(this);});
  connect(ui->add,&QPushButton::clicked,[this](){this->addDevice();});
connect(ui->delete_,&QPushButton::clicked,[this](){this->removeDevice();});
}

DVMainWindow::~DVMainWindow()
{
  delete ui;
}
void DVMainWindow::UpdateContents(DVMainWindow* this_){

  std::map<int,Device> device_list;
  device_list=DeviceManager::getDeviceList(Device::DeviceType::all);
  this_->ui->devicelist->clear();
  this_->ui->devicelist->clearSelection();
  char buffer[512]{};

  for(auto & i: device_list){
    snprintf(buffer,sizeof(buffer),"type: %-10dID: %-20sName: %-35sState: %-5d",i.second.getType(),
             i.second.getIDString().c_str(),i.second.getName().c_str(),i.second.getState());
    this_->ui->devicelist->addItem(QString(buffer));
  }

}

void DVMainWindow::addDevice(){
  int row=this->ui->TypeChoose->currentRow();
  switch( row){
    case 0:
      DeviceManager::addDevice(Device::keyboard);
      break;
    case 1:
      DeviceManager::addDevice(Device::disk);
      break;
    case 2:
      DeviceManager::addDevice(Device::printer);
      break;
    case 3:
      DeviceManager::addDevice(Device::terminal);
      break;
    default:break;
    }
    this->UpdateContents(this);



}
void DVMainWindow::removeDevice(){
  char buffer[512]={};
  int id;
  QString item=this->ui->devicelist->currentItem()->text();
  QStringList list1=item.split(' ',Qt::SkipEmptyParts);
  DeviceManager::removeDevice(list1.at(3).toInt());

}
