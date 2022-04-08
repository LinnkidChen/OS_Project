## 浅浅决定一下指令集
1. C time/clock
* 模拟使用CPU进行一定时间的计算
* 可选：传入参数进行具体的计算
* clock: int类型 表示占用的时钟数。
2. K time -Maxsize=1024
* 在内存中找一块区域将键盘输入写入。
* time :使用的时钟数
* MaxSize: 供写入的最大容量 默认为1024
3. P -offset=0 -size=1024
* 打印机输出内存中的一块区域
* offset: 开始打印的地址=进程分配到的起始地址+offset，offset默认为0
* size: 打印的块的大小，默认为1024
4. R filedir -offset=0 -size=-1
* 从文件读入一块数据到内存中
* filedir: 文件路径
* offset: 开始读入的地址=文件起始地址+offset，offset默认为0
* size: 读取的块的大小，默认为-1，即整个文件
5. W filedir -offset=0 -size=-1
* 将内存中的一块数据写入到文件中
* filedir: 文件路径
* offset: 开始写入的地址=文件起始地址+offset，offset默认为0
* size: 写入的块的大小，默认为-1，即整个输出缓冲区。
