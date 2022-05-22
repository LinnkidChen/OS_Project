#ifndef FILEWINDOW_H
#define FILEWINDOW_H
#include <QWidget>
#include <QTreeWidgetItem>
typedef std::string String;

QT_BEGIN_NAMESPACE
namespace Ui { class fileWindow; }
QT_END_NAMESPACE

class fileWindow : public QWidget
{
    Q_OBJECT

public:
    fileWindow(QWidget *parent = nullptr);
    ~fileWindow();
    QTreeWidgetItem* addToFolder(QTreeWidgetItem*, String);
    void removeItem(QTreeWidgetItem*);
    void showFileTree();
    void deleteItemFromList(QTreeWidgetItem*);

    std::vector<QTreeWidgetItem*> folderList;
    std::vector<QTreeWidgetItem*> fileList;

    std::vector<QTreeWidgetItem*> actItem;
    std::vector<String> actPath;

public slots:
    void checkself(QTreeWidgetItem*, int);
    void saveFile();
    void deleteFile();

private:
    Ui::fileWindow *ui;
};
#endif // FILEWINDOW_H
