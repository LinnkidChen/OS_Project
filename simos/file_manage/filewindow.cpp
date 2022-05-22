#include "filewindow.h"
#include "window_interface.h"
#include "ui_filewindow.h"

fileWindow::fileWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::fileWindow)
{
    ui->setupUi(this);
    showFileTree();

    /*绑定事件*/
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(checkself(QTreeWidgetItem*, int)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(deleteFile()));
    //string s = qstr.toStdString();
    //QString Qfilename = QString::fromStdString(Sfilename);
}

void fileWindow::showFileTree() {
    /*请求到全部文件列表*/
    std::vector<std::vector<String>> file = win_iter_dir();

    QTreeWidgetItem* parent = NULL;
    for (int i = 0; i < file.size(); i++) {
        for (int j = 0; j < file[i].size() - 1; j++) {
            if (j == 0) {
                parent = ui->treeWidget->topLevelItem(0);
            }
            /*将文件夹添加进folderList*/
            if(folderList.size() == 0)
                folderList.push_back(parent);
            else {
                for (int x = 0; x < folderList.size(); x++) {
                    if (folderList[x] == parent)
                        break;
                    if (x == folderList.size() - 1)
                        folderList.push_back(parent);
                }
            }
            /*构建文件树*/
            if (file[i][j + 1] != "#") {
                parent = addToFolder(parent, file[i][j+1]);
                /*将文件添加进fileList*/
                if (j == file[i].size() - 2) {
                    if(fileList.size() == 0)
                        fileList.push_back(parent);
                    else {
                        for (int x = 0; x < fileList.size(); x++) {
                            if (fileList[x] == parent)
                                break;
                            if (x == fileList.size() - 1)
                                fileList.push_back(parent);
                        }
                    }
                }
            }
        }
    }
}

QTreeWidgetItem* fileWindow::addToFolder(QTreeWidgetItem* parent, String Sfilename) {
    QString Qfilename = QString::fromStdString(Sfilename);
    int size = parent->childCount();
    for (int i = 0; i < size; i++) {
        QTreeWidgetItem* child = parent->child(i);
        QString childname = child->text(0);
        if (childname == Qfilename) {
            return child;
        }
    }
    QTreeWidgetItem* newitem = new QTreeWidgetItem(QStringList() << Qfilename);
    parent->addChild(newitem);
    return newitem;
}

void fileWindow::removeItem(QTreeWidgetItem* item) {
    int count = item->childCount();
    if (count == 0)//没有子节点，直接删除
    {
        if (item != ui->treeWidget->topLevelItem(0)) {
            deleteItemFromList(item);
            delete item;
        }
        return;
    }
    for (int i = 0; i < count; i++)
    {
        QTreeWidgetItem* childItem = item->child(0);//删除子节点
        removeItem(childItem);
    }
    if (item != ui->treeWidget->topLevelItem(0)) {
        deleteItemFromList(item);
        delete item;//最后将自己删除
    }
}

void fileWindow::saveFile() {
    if (actPath.size() != 0) {
        /*获取输入内容*/
        String newContent = ui->textEdit->toPlainText().toStdString();
        /*发送输入内容和文件路径（需判断该路径末是不是文件）*/
        for (int i = 0; i < fileList.size(); i++) {
            if (fileList[i] == actItem[actItem.size() - 1]) {
                //发送路径和内容
                win_save_file(actPath,newContent,newContent.length());
                break;
            }
        }
    }
}

void fileWindow::deleteFile() {
    if (actPath.size() != 0) {
    /*首先将文件路径发送*/
        win_del_file(actPath);
    /*其次删除tree的节点,将操作路径清空*/
        if (actItem[actItem.size() - 1] != ui->treeWidget->topLevelItem(0)) {
            removeItem(actItem[actItem.size() - 1]);
            actItem.clear();
            actPath.clear();
        }
    }
}

void fileWindow::checkself(QTreeWidgetItem* item, int count) {
    actPath.clear();
    actItem.clear();
    ui->textEdit->clear();
    QTreeWidgetItem* temp = item;
    /*点击后取得文件或文件夹路径放在actPath中*/
    if (temp != ui->treeWidget->topLevelItem(0)) {
        /*加入自身*/
        if (temp->childCount() == 0) {
            for (int x = 0; x < folderList.size(); x++) {
                if (folderList[x] == temp) {
                    actPath.insert(actPath.begin(), "#");
                    actPath.insert(actPath.begin(), temp->text(0).toStdString());
                    actItem.insert(actItem.begin(), temp);
                    break;
                }
                if (x == folderList.size() - 1) {
                    actPath.insert(actPath.begin(), temp->text(0).toStdString());
                    actItem.insert(actItem.begin(), temp);
                }
            }
        }
        else {
            actPath.insert(actPath.begin(), temp->text(0).toStdString());
            actItem.insert(actItem.begin(), temp);
        }
        /*加入父文件夹*/
        for (; temp->parent()->text(0).toStdString() != "root";) {
            actPath.insert(actPath.begin(), temp->parent()->text(0).toStdString());
            actItem.insert(actItem.begin(), temp->parent());
            temp = temp->parent();
        }
    }
    actPath.insert(actPath.begin(), "root");
    actItem.insert(actItem.begin(), ui->treeWidget->topLevelItem(0));

    /*判断点击目标是否是文件*/
    for (int i = 0; i < fileList.size(); i++) {
        if (fileList[i] == actItem[actItem.size() - 1]) {
            /*是文件则请求文件内容并展示*/
            String text = win_get_file(actPath);
            ui->textEdit->append(QString::fromStdString(text));
            break;
        }
    }
    //for (int i = 0; i < actPath.size(); i++) {
    //    ui.textEdit->append(QString::fromStdString(actPath[i]));
    //}
    //for (int i = 0; i < actItem.size(); i++) {
    //    ui.textEdit->append(actItem[i]->text(0));
    //}
}

void fileWindow::deleteItemFromList(QTreeWidgetItem* item) {
    for (int x = 0; x < fileList.size(); x++) {
        if (fileList[x] == item) {
            fileList.erase(fileList.begin() + x);
            return;
        }
    }
    for (int x = 0; x < folderList.size(); x++) {
        if (folderList[x] == item) {
            folderList.erase(folderList.begin() + x);
            return;
        }
    }
}

fileWindow::~fileWindow()
{
    delete ui;
}

