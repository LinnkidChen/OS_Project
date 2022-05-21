#include "memorysimulatewindow.h"
#include "simos/MainWindow.h"
#include "ui_memorysimulatewindow.h"



MemorySimulateWindow::MemorySimulateWindow(MainWindow *mainWnd, QWidget *parent)
    : QWidget(parent),
      ui(std::make_unique<Ui::MemorySimulateWindow>()),
      mainWindow(mainWnd)
{
      ui->setupUi(this);
      this->hide();

  connect(ui->btnBack, &QPushButton::released, this,
          &MemorySimulateWindow::SwitchToMainWindow);

      ui->LRU_information->setFontPointSize(12);
      QString str;
      str =  ui->InputProcessName->toPlainText();
      connect(ui->pushButton,&QPushButton::clicked,[=]()
          {

              int a =0,b = 4096;
              std::vector<size_t> res;
              srand((int)time(0));
              int num = rand()%4 + 6;
              srand((int)time(0));  //利用系统时钟，产生不同的随机数种子,把0换成NULL也行
              for(int i = 1 ; i <= num; i++)
              {
                  int number = rand()% (b - a) + a;
                  res.push_back(number);

              }

              for(int i = 0 ; i < num; i++)
              {
                  ui->PageNums->insertPlainText(QString::number(res[i]));
                  ui->PageNums->insertPlainText(" ");
              }

              ui->LRU_information->insertPlainText("页面号引用串: ");
              for(int i = 0 ; i < num; i++)
              {
                  ui->LRU_information->insertPlainText(QString::number(res[i]));
                  ui->LRU_information->insertPlainText(" ");
              }
              ui->LRU_information->append("\n");
              LRUCache(4);

              for(int i = 0; i < num; i++)
              {

                  ui->LRU_information->insertPlainText("虚拟内存:\n");
                  put(res[i]);
                  Node* p = qhead->right;
                  while(p != qtail)
                  {
                      ui->LRU_information->insertPlainText(QString::number(p->key));
                      ui->LRU_information->insertPlainText(" ");
                      p = p->right;
                  }
                  ui->LRU_information->append("\n");
                  ui->LRU_information->insertPlainText("是否发生置换: ");
                  ui->LRU_information->insertPlainText(QString::number(flag));
                  ui->LRU_information->append("\n");


              }
                  ui->LRU_information->append("\n");

              ui->LRU_information->insertPlainText("缺页率: ");
              double rate = replace_num/num;
              QString qrate = QString::number(100*rate);
              ui->LRU_information->insertPlainText(qrate+"%");
              res.clear();
              qhash.clear();
              replace_num = 0;
              flag = 0;
              qhead = new Node(-1);
              qtail = new Node(-1);
              qhead->right = qtail;
              qtail->left = qhead;


          });
      connect(ui->clearPageNums,&QPushButton::clicked,[=]()
          {

              ui->PageNums->clear();
          });
      connect(ui->clearLRU_information,&QPushButton::clicked,[=]()
          {

              ui->LRU_information->clear();
          });

}

MemorySimulateWindow::~MemorySimulateWindow() = default;


void MemorySimulateWindow::SwitchToMainWindow() {
  this->hide();
  mainWindow->show();
}

std::vector<int> MemorySimulateWindow::create_rand_pagenums_vector()//取得[a,b)的随机整数
{
    int a =0,b = 4096;
    std::vector<int> res;
    srand((int)time(0));
    int num = rand()% 16 + 10;
    srand((int)time(0));  //利用系统时钟，产生不同的随机数种子,把0换成NULL也行
    for(int i = 1 ; i <= num; i++)
    {
        int number = rand()% (b - a) + a;
        //cout<<number<<" ";
        res.push_back(number);
    }
    return res;
}

void MemorySimulateWindow::removeNode(Node* p)
{
    p->left->right = p->right;
    p->right->left = p->left;
}

void MemorySimulateWindow::insertNode(Node* p)
{
    p->right = qhead->right;
    p->left = qhead;
    qhead->right->left = p;
    qhead->right = p;
}

void MemorySimulateWindow::LRUCache(int capacity)
{
    n = capacity;
    qhead = new Node(-1);
    qtail = new Node(-1);
    qhead->right = qtail;
    qtail->left = qhead;
}

int MemorySimulateWindow::get(int key)
{
    if(qhash.count(key) == 0)return -1;
    Node* tmp = qhash[key];
    removeNode(tmp);
    insertNode(tmp);
    return tmp->value;
}

void MemorySimulateWindow::put(int key)
{
    flag = 0;
    if(qhash.count(key))
    {

        Node* tmp = qhash[key];
        removeNode(tmp);
        insertNode(tmp);
    }
    else{
        if(qhash.size() == n)
        {
            flag = 1;
            replace_num++;
            Node* tmp = qtail->left;
            removeNode(tmp);
            qhash.erase(tmp->key);
            delete tmp;
        }
        Node* tmp = new Node(key);
        qhash[key] = tmp;
        insertNode(tmp);
    }
}
