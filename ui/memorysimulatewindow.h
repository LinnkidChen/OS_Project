#ifndef MEMORYSIMULATEWINDOW_H
#define MEMORYSIMULATEWINDOW_H

#include <QWidget>
#include<vector>
#include<unordered_map>

namespace Ui {
class MemorySimulateWindow;
}

class MainWindow;

class MemorySimulateWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MemorySimulateWindow(MainWindow *mainWnd,
                                      QWidget *parent = nullptr);
    ~MemorySimulateWindow();
       std::vector<int> create_rand_pagenums_vector();//取得[a,b)的随机整数
       struct Node{
           int key,value;
           Node *left,*right;
           Node(int _key): key(_key),left(NULL),right(NULL){}
       };
       int n;
       double replace_num;
       double flag = 0;
       std::unordered_map<int,Node*> qhash;
       struct Node *qhead,*qtail;
       void removeNode(Node* p);
       void insertNode(Node* p);
       void LRUCache(int capacity);
       int get(int key);
       void put(int key);

    private slots:
      void SwitchToMainWindow();

private:
    std::unique_ptr<Ui::MemorySimulateWindow> ui;
    MainWindow *mainWindow;
};

#endif // MEMORYSIMULATEWINDOW_H


