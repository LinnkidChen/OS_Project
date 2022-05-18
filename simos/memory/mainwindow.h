#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"QString"
#include"QDebug"
#include"QPushButton"
#include<vector>
#include<unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    std::vector<int> create_rand_pagenums_vector();//取得[a,b)的随机整数
    ~MainWindow();
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

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
