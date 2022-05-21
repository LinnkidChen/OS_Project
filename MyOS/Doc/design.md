# 文件管理

## 一、接口

* `Handle file_open(const char *filename, int permission)`

  > Handle的定义位于`file_manage.h`头文件中，用于唯一标识一个打开的文件。错误则返回-1
  >
  > 文件权限`permission`有如下种类：
  >
  > 1. `CREATE`创建，新建一个文件，若文件已存在则忽略此标志
  > 2. `TRUNC`截断，打开并清空文件原内容，若文件不存在则创建
  > 3. `APPEND`追加，打开文件并将文件指针置于文件末尾
  > 4. `READ`读，可以读取文件内容。即可以使用函数`file_read`
  > 5. `WRITE`写，可以向文件中写入内容。即可以使用函数`file_write`

* `int file_seek(Handle h, int offset, int mode)`

  > 移动文件指针的位置
  >
  > `mode`有如下三类：
  >
  > 1. `FILE_SEEK_SET`文件开头
  >
  >    **注意：当以APPEND模式打开文件时，文件指针初始将位于文件末尾但使用此mode依然会从文件开头计算**
  >
  > 2. `FILE_SEEK_CUR`当前位置
  >
  > 3. `FILE_SEEK_END`文件末尾

* `int file_write(Handle h, size_t length, const void *str)`

  > 写入内容（覆盖），返回写入长度（-1代表错误）

* `int file_read(Handle h, size_t length, void *str)`

  > 读出内容（覆盖），返回读出长度（-1代表错误）

* `int file_close(Handle)`

  > 关闭一个文件

## 二、设计

### 1. 文件接口

* `fopen/fclose`
  * 维护一个已打开文件的表
  * 表项包含FCB，文件指针ptr，以及文件权限
  * 关闭时删掉表项
* `fseek`
  * 根据mode改变ptr值
* `fwrite/fread`
  * 两个函数根据ptr的值确定正确的磁盘块和块内偏移量，然后调用磁盘的函数
    * 举例：seek到2001，代表打开第三个磁盘块（编号2）然后seek到位置1
  * 然后创建文件夹就是写入一个FCB

### 2. 磁盘管理

* 地址系统
  * 磁盘块数为64000，总磁盘大小64MB
  * 合法的地址范围是`00000'000`~`63999'999`（八位），每个地址的最低三位是块内地址，前面是块地址
  * FCB里存的地址后三位应当都是0
    * 一个用于存放间接索引的块能存放125个地址
    * 一个用于存放FCB的块能存放至少10个FCB
* 磁盘管理系统
  * 维护一个磁盘分配表，用于标记哪些块是被占据的
  * 当文件需要一个新的块，应当找个未占据的块给它
  * 磁盘系统应当封装的操作：
    * 写入内容
    * 读出内容
    * 检测块容量
    * 读入一行（不对外）
    * 写一行（不对外）
    * 搜索一行内容（不对外）
* 块在以下情形时需要被分配给文件：
  * 新建时
  * 文件的直接索引部分被占满，需要存放间接索引块时
  * 文件现有的内容与`fwrite`写入的内容之和超过文件容量时
* 块在以下情形时应当被回收：
  * 使用`TRUNC`打开一个文件时，文件所属的全部块被回收
  * 一个文件被删除时，文件所属的全部块被回收

### 3. 程序结构（头文件定义）

* `file_manage.h`

  ```C++
  enum FILE_PERMISSION {
      F_CREATE = 1,
      F_TRUNC = 2,
      F_ADDEND = 4,
      F_READ = 8,
      F_WRITE = 16
  };
  
  enum SEEK_MODE { FILE_SEEK_SET = 0, FILE_SEEK_CUR = 1, FILE_SEEK_END = 2 };
  
  Handle file_open(const char *filename, int permisson);
  int file_seek(Handle h, int offset, int mode);
  int file_write(Handle h, size_t length, const void *str);
  int file_read(Handle h, size_t length, void *str);
  int file_close(Handle h);
  ```

* `disk.h`

  ```c++
  #define BLOCK_SIZE 1000
  
  #define BLOCK_NUM 64000
  
  #define ADDR_PER_BLOCK 125
  
  #define DISK(X) ("DISK/" + std::to_string((X) / BLOCK_SIZE) + ".disk")
  
  // 写入磁盘块，返回成功写入的长度（块容量不超过块大小）
  size_t disk_write(Addr addr, const char *str, size_t length);
  
  // 读出内容，返回成功读取的长度（块容量不超过块大小）
  size_t disk_read(Addr addr, char *str, size_t length);
  
  // 返回块当前已用空间
  int disk_size(Addr addr);
  
  // 写入（覆盖）一行内容，line为行号，-1代表追加
  size_t disk_writeline(Addr addr, String str, size_t length, int line);
  size_t disk_writeline(Addr addr, int str, int line);
  
  // 读取一行内容，line为行号
  String disk_readline(Addr addr, int line);
  
  // 搜索文件名
  int disk_searchline(Addr addr, String name);
  ```

* `disk_manage.h`

  ```c++
  // 返回一个未被占据的块的地址，并标记为已占用，-1代表出错
  Addr assign_block();
  
  // 回收一个块
  int recycle_block(Addr addr);
  ```

* `FCB.h`

  ```C++
  #define INDEX_NUM 4
  
  class FCB {
    public:
      FCB(String name, bool isDir, bool isRunable);
      FCB(String str);
      int check_size();
      inline void set_index(Addr, int, bool);
      inline int file_size();
      inline bool is_dir();
      inline bool is_runable();
      inline Addr direct_index(int);
      inline Addr indirect_index(int);
      String to_string();
      String create(String);
      
    private:
      const String filename;         // 文件名，最大长度固定
      int filesize;                  // 文件大小
      bool dir;                      // 是否为文件夹
      bool runable;                  // 是否为可执行文件
      Addr directIndex[INDEX_NUM];   // 直接索引块
      Addr indirectIndex[INDEX_NUM]; // 间接索引块
  };
  
  ```

* `file.h`

  ```c++
  struct o_file {
      FCB *fcb;
      std::vector<Addr> *disks;
      int Handle;
      int ptr;
      int permission;
      Addr address;
      int line;
  };
  typedef struct o_file o_file;
  
  Handle add(FCB *fcb, int permission, Addr address, int line);
  
  // 根据ptr定位磁盘块
  Addr locate(o_file *f);
  
  // 扩张一个块
  void expand_a_block(o_file *f);
  ```