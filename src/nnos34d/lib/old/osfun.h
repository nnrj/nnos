/**
 * 外部函数声明（来自汇编文件osfun.asm的函数）
 **/

void write_mem8(int addr, int data);    //声明write_mem8函数，此函数使用汇编语言编写，编译时连接，末尾数字8指明一次写入8位
void io_hlt();                          //声明CPU停机等待函数，此函数使用汇编语言编写，编译时连接
void io_cli();                          //屏蔽中断函数声明（只能屏蔽可屏蔽中断）
void io_out8();                         //向指定设备输出数据的函数声明（用于向指定端口输出数据），，末尾数字8指明一次输出8位
void io_store_eflags(int eflags);       //中断指令无JN、JNC,只能通过读取EFLAGS,检查第9位（中断标志位）的值，进而判断
int io_load_eflags();                   //读取中断前保存的标志位信息，恢复现场。CPU中有一个由16位flag寄存器扩充而来的32位寄存器eflag，用作进位、中断等标志寄存器。
void load_gdtr(int limit,int addr);     //加载GDT地址到GDTR段寄存器
void load_idtr(int limit,int addr);     //加载IDT地址到IDTR段寄存器

/*=====已废弃的声明=====================================================================================================*/
//char get_vram_info();

