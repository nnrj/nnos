/**
 * ===================NNOS-nnos.h=============================
 *【系统函数声明】
 * -----------------------------------------------------------
 * ·日期：2019-3-23      作者：年年软件
 * ===========================================================
**/
/**
 * 
 **/
/*头文件引用*/			     //0x280000~0x2ffffff为*.h
#include <stdio.h>		    //标准输入输出函数，本系统可使用sprintf()(此函数与操作系统无关)，不能用printf()
#include <string.h>			//字符串处理函数，本系统将使用本函数库中与操作系统无关的函数
#include "sysdef.h"		    //系统常量
#include "systructual.h"    //系统结构体

/*=====内部函数声明======================================================================================================*/
/*bootpack.c*/
////int keywinOff(SHEET *key_win,SHEET *sht_win, int cur_c, int cur_x);
////int keywinOn(SHEET *key_win,SHEET *sht_win, int cur_c);
void keywinOff(SHEET *keyWin);
void keywinOn(SHEET *keyWin);
void change_wtitle8(SHEET *sht, char act);
SHEET *openConsole(SHTCTL *shtctl);
void closeConstask(TASK *task);
void closeConsole(SHEET *sht);
void newConsole(SHTCTL *shtctl,SHEET *keyWin,int scrx,int scry);

/*=====外部函数声明======================================================================================================*/
/*osfun.asm的函数声明*/
void write_mem8(int addr, int data);    //[已废弃]声明write_mem8函数，此函数使用汇编语言编写，编译时连接，末尾数字8指明一次写入8位
void io_hlt();                          //声明CPU停机等待函数，此函数使用汇编语言编写，编译时连接
void io_cli();                          //屏蔽中断函数声明（只能屏蔽可屏蔽中断）
void io_sti();  
void io_stihlt();
int io_in8(int port);                       
void io_out8(int port,int data);        //向指定设备输出数据的函数声明（用于向指定端口输出数据），，末尾数字8指明一次输出8位
void io_store_eflags(int eflags);       //中断指令无JN、JNC,只能通过读取EFLAGS,检查第9位（中断标志位）的值，进而判断
int io_load_eflags();                   //读取中断前保存的标志位信息，恢复现场。CPU中有一个由16位flag寄存器扩充而来的32位寄存器eflag，用作进位、中断等标志寄存器。
void load_gdtr(int limit,int addr);     //加载GDT地址到GDTR段寄存器
void load_idtr(int limit,int addr);     //加载IDT地址到IDTR段寄存器
void asm_inthandler21();
void asm_inthandler27();
void asm_inthandler2c();
void asm_inthandler20();
void asm_inthandler0d(); //一般异常中断处理
void asm_inthandler0c(); //栈异常中断处理
void asm_inthandler00();
void asm_inthandler01();
void asm_inthandler03();
void asm_inthandler04();
void asm_inthandler0a();
void asm_inthandler0b();
void asm_inthandler05();
void asm_inthandler08();
int load_cr0();
void store_cr0(int cr0);
void load_tr(int tr);
void taskswitch3();
void taskswitch4();
void farjmp(int esp,int cs);
//void asm_sysprint();
void farcall(int eip,int cs);
void asm_sys_api();
//void run_app(int eip,int cs,int esp,int ds);
void run_app(int eip,int cs,int esp,int ds,int *tss_esp0); 
void asm_end_app(); 
void asm_shutdown(); //关机

/*gdtidi.c的函数声明*/
void initGdtIdt(); //初始化GDT和IDT;
void setSegmDesc(SEGMENT_DESCRIPTOR *segmentDescriptor,unsigned int limit,int base,int accessPre); //全局描述符设置
void setGateDesc(GATE_DESCRIPTOR *gateDescriptor,int offset,int selector,int accessPre); //中断描述符设置

/*interrupt.c的函数声明*/
void initPIC(); //初始化PIC
void inthandler21(int *esp); //PS/2键盘中断处理器
void inthandler2c(int *esp); // //PS/2鼠标中断处理器
void inthandler27(int *esp); //针对PIC0的不完全中断补丁
void initPIT(); //初始化PIT(可编程间隔定时器)
void inthandler20(int *esp); //处理IRQ0中断，设置定时器
int *inthandler0c(int *esp);
int *inthandler0d(int *esp);
int *inthandler00(int*esp);
int *inthandler01(int*esp);
int *inthandler03(int*esp);
int *inthandler04(int*esp);
int *inthandler0a(int*esp);
int *inthandler0b(int*esp);
int *inthandler05(int*esp);
int *inthandler08(int*esp);

/*fifo.c的函数声明*/
void initFIFOBuffer(FIFO_BUFFER *fifoBuffer,unsigned char *buffer,int bufferSize); //初始化缓冲区（8位）
int putFIFOBuffer(FIFO_BUFFER *fifoBuffer,unsigned char data); //向缓冲区写入数据
int getFIFOBuffer(FIFO_BUFFER *fifoBuffer); //从缓冲区读取数据
int getFIFOBufferStatus(FIFO_BUFFER *fifoBuffer); //获取缓冲区数据数量
void initFIFOBuffer32(FIFO_BUFFER32 *fifoBuffer,int *buffer,int bufferSize,TASK *task); //初始化32位缓冲区
int putFIFOBuffer32(FIFO_BUFFER32 *fifoBuffer,int data); //向32位缓冲区写入数据
int getFIFOBuffer32(FIFO_BUFFER32 *fifoBuffer); //从32位缓冲区读取数据
int getFIFOBufferStatus32(FIFO_BUFFER32 *fifoBuffer); //获取32位缓冲区数据数量

/*memery.c的函数声明*/
unsigned int memeryCheck(unsigned int start,unsigned int end); //内存检查函数
unsigned int memtest_sub(unsigned int start,unsigned int end); //内存检查子函数(实现见osfun.asm)
void initMemeryManager(MEMERY_LIST *memeryList); //初始化内存块列表
unsigned int memeryTotal(MEMERY_LIST *memeryList); //获取内存总剩余量[准备废弃]
unsigned int memeryAlloc(MEMERY_LIST *memeryList,unsigned int size); //分配内存
int memeryFree(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size); //释放内存
void deleteBlock(MEMERY_LIST *memeryList,int i); //删除空内存表项
void addBlock(MEMERY_LIST *memeryList,int i); //插入内存表项
void sortMemery(MEMERY_LIST *memeryList,int low,int high); //内存表项按内存块大小快速排序（递增）
void sortAddr(MEMERY_LIST *memeryList,unsigned int low,unsigned int high); //内存表项按内存地址大小快速排序（递增）
unsigned int memeryAlloc4k(MEMERY_LIST *memeryList,unsigned int size); //内存空间分配前向上取整，最小4K(0x)，减少碎片
unsigned int memeryFree4k(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size); //内存空间释放按4K向上取整
void clearMemery(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size);

/*graphics.c的函数声明*/
//static WINDOW autoWindow[100];
void initPalette();	//画板初始化函数,设置画板前执行CLI屏蔽中断
void setPalette(int start,int end,unsigned char *rgb); //设置画板函
void pointDraw8(char *vram,short scrx,PIX_POINT point); //像素点绘制函数，vram为显卡首地址，PIX_POINT类型定义见systrual.h
//void boxDraw8(char *vram,short scrx,int bx0,int by0,int bx1,int by1,unsigned int vcolor); 
void boxDraw8(unsigned char *vram,int scrx,int bx0,int by0,int bx1,int by1,unsigned char vcolor);//矩形区域绘制函数，vram为显卡首地址，Box类型定义见systrual.h
void boxDrawx(char *vram,short scrx,BOX *box); //批量矩形绘制函数
void initDesk(char *vram,short scrx,short scry);//初始化桌面函数，SCREEN结构体类型见systrual.h
//void fontDraw8(char *vram,short scrx, int fx, int fy, char vcolor, char *font); //字体渲染引擎
void fontDraw8(char *vram,int scrx, int fx, int fy, char vcolor, char *font); //字体渲染引擎
void fontDraw32(char *vram,int scrx,int fx,int fy,char vcolor,char *font1,char *font2); //GB2312字体渲染引擎
//void wordsDraw8(char *vram,short scrx,int fx,int fy,char vcolor,unsigned char *words); //字符串批量渲染引擎
void wordsDraw8(char *vram,int scrx,int fx,int fy,char vcolor,unsigned char *words); //字符串批量渲染引擎
void initMouseCursor8(char *cusorGraph,char curBackColor); //初始化鼠标指针函数
void pictureDraw8(char *varm,short scrx,PICTURE picture); //图形渲染函数
//void windowDraw8(char *vram,short scrx,WINDOW window); //窗口绘制函数
//void windowDraw8(char *vram,short scrx,WINDOW window,int focus); //窗口绘制函数
//void windowDraw8(char *vram,short scrx,WINDOW window,int focus,int area);
void windowDraw8(char *vram,short scrx,WINDOW window,int focus,int area);
//void createWindow(unsigned char *coverBuffer, int width, int height, char *caption,int type); //快速窗口创建
void createWindow(unsigned char *coverBuffer, int width, int height, char *caption,int type,int focus);
//WINDOW *createWindow(unsigned char *coverBuffer,int width,int height,char *caption,int type,int focus);
//WINDOW createWindow(unsigned char *coverBuffer,int width,int height,char *caption,int type,int focus);
void labelDraw(SHEET *cover,int x,int y,int foreColor,int backColor,char *s,int len); //标签绘制函数
void makeTextBox8(SHEET *sht,int x0,int y0,int width,int htight,int c); //绘制文本框
//void refreshWindowCaption(unsigned char *coverBuffer,int width,int height,char *caption,int backColor); //刷新窗口标题
//void windowCaptionDraw8(char *vram,short scrx,WINDOW *window,int focus);
//void refreshWindowCaption(unsigned char *coverBuffer,WINDOW window,int focus);
void refreshWindowCaption(unsigned char *coverBuffer, int width, int height, char *caption,int type,int focus);
void syslinewin(SHEET *sht,int x0,int y0,int x1,int y1,int vcolor); //窗体直线绘制函数
void refreshWindowCaptionx(SHEET *sht,int focus);

/*devices.c的函数声明*/
void initScreen(SCREEN *screen,BOOT_INFO *bootInfo); //屏幕信息初始化函数
//char getKeyCode(); //获取键盘码
void waitKBCReady(); //等待键盘控制电路准备完毕函数
void initKeyboardCMD(); //初始化键盘控制电路函数
void enableMouse(MOUSE_CURSOR *mouseCursor); //激活鼠标函数
int decodeMouse(MOUSE_CURSOR *mouseCursor,unsigned char data); //鼠标解码引擎
void enableMouse32(FIFO_BUFFER32 *FIFOBuffer,int dataBase,MOUSE_CURSOR *mouseCursor); //初始化键盘控制电路函数,32位
void initKeyboardCMD32(FIFO_BUFFER32 *FIFOBuffer,int dataBase); //激活鼠标函数，32位
void sysHLT();	//停机等待函数

/*cover.c的函数声明*/
COVER_LIST *initCoverList(MEMERY_LIST *memeryList,SCREEN screen);
COVER *coverAlloc(MEMERY_LIST *memeryList,COVER_LIST *coverList);
void setCover(COVER *cover,PICTURE picture,int transColor);
void updateCover(COVER_LIST *coverList,COVER *cover,int order);
void coverRefresh(COVER_LIST *coverList);
void coverMove(COVER_LIST *coverList,COVER *cover,int vx,int vy); //移动图层
void coverFree(MEMERY_LIST *memeryList,COVER_LIST *coverList,COVER *cover);

/*timer.c的函数声明*/
void initTimer(TIMER *timer,FIFO_BUFFER32 *FIFOBuffer,int data);
TIMER *timerAlloc();
void freeTimer(TIMER *tiemr);
void setTimer(TIMER *timer,unsigned int timeout);
//void reTimer();
//void reTimer_old();
//void sortTimerList();
//void reTImer_old2();
void setTest480(FIFO_BUFFER32 *fifo,int mode);
int timerCancel(TIMER *timer);
void timerCancelAll(FIFO_BUFFER32 *fifo);

/*task.c*/
extern struct TIMER *taskTimer;
/* void initTask();
void taskSwitch(); */
TASK *initTaskList(MEMERY_LIST *memeryList); //任务初始化
TASK *taskAlloc(); //任务申请
//void taskRun(TASK *task); //任务启动
//void taskRun(TASK *task,int priority); //任务启动，指定优先级，指定为0时，不改变优先级
void taskRun(TASK *task,int level,int priority); //任务启动，指定优先级，指定为0时，不改变优先级；指定优先级队列小于0时不改变队列
void taskSwitchLimit(); //任务手动切换
void taskSwitch(); //任务自动切换
void taskSleep(TASK *task); //任务休眠
void addTask(TASK *task);
void removeTask(TASK *task);
void taskSwitchSub();
void idleTask(); //空闲任务
TASK *taskNow();

/*window.c*/
void initWindowList(WINDOW_LIST *windowList);
WINDOW *windowAlloc();
void setWindowBox(WINDOW_LIST *windowList,WINDOW *window,BOX *box);
void setWindowCaption(WINDOW *window,unsigned char backColor);
void setWindowFocus(WINDOW_LIST *windowList,WINDOW *window);
WINDOW *getWindow(WINDOW_LIST *windowList,int id);

/*console.c*/
//void consoleTask(SHEET *sheet); //控制台程序
void consoleTask(SHEET *sheet,unsigned int totalFreeSize);
//int newCMDLine(int cursorY,SHEET *sheet); //控制台换行函数
void newCMDLine(CONSOLE *console); //换行命令
void sysprint(CONSOLE *console,int charCode,char movFlag); //字符输出
void commandCMD(CONSOLE *console,int *fat,char *command); //comman执行
void clsCMD(CONSOLE *console); //cls命令
void dirCMD(CONSOLE *console); //dir命令
void typeCMD(CONSOLE *console,int *fat,char *command); //查看文件内容
void delCMD(CONSOLE *console,int *fat,char *command); //删除文件
void sysinfoCMD(CONSOLE *console); //显示系统信息
void versionCMD(CONSOLE *console);
void memCMD(CONSOLE *console); //显示内存信息
////void runCMD(CONSOLE *console,int *fat,char *command); //运行可执行文件
int runCMD(CONSOLE *console,int *fat,char *command); //运行可执行文件
void exitCMD(CONSOLE *console, int *fat); //退出控制台
void startCMD(CONSOLE *console, char *command); //在新窗口运行程序
void syslangCMD(CONSOLE *console,char *command); //切换语言模式
void shutdownCMD(CONSOLE *console, char *command); //关机
void invalidCMD(CONSOLE *console,char *command); //无效的命令
void printLine(CONSOLE *console); //打印固定长度横线，长度为32*8像素
void printLinex(CONSOLE *console,int len); //打印指定长度横线
void printAddress(CONSOLE *console,unsigned char addr); //打印地址
void sysprintl(CONSOLE *console,char *str); //打印一行字符串
void sysprintx(CONSOLE *console,char *str,int len); //打印指定长度字符串
//void sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax);
int *sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax);
//void inthandler0d0(int *esp); //应用程序一般异常中断处理器
int *inthandler0d0(int *esp); //应用程序一般异常中断处理器
int *inthandler0c(int*esp); //栈异常中断处理器

/*file.c*/
void readFAT(int *fat,unsigned char *img); //解码并装载FAT
void loadFile(int cluster,int size,char *file,int *fat,char *img); //装载文件
FILE_INFO *searchFile(char *fileName); //文件查找（默认）
//FILE_INFO *searchFile(char *fileName,CONSOLE *console);
////FILE_INFO *file_search(char *name); //文件查找（默认）
FILE_INFO *file_search2(char *name, struct FILE_INFO *fileInfo, int max); //文件查找，可指定
//char *loadFile2(int cluster, int *psize, int *fat);
int *deleteFile(char *fileName); //文件删除

/*service.c*/
//void tssBMain();
//void tssBMain(SHEET *sht_back);
void tssBMain(SHEET *sht_win_b); //测试程序

/*string.c*/
void convertToUppercase(char *str); //转为大写字母
void convertToLowercase(char *str); //转为小写字母
//void printLine(); //打印默认横线，长度32
//void printLinex(int len); //打印指定长度横线

/*others.c的函数声明*/
SHTCTL *shtctl_init(MEMERY_LIST *memman, unsigned char *vram, int xsize, int ysize);
SHEET *sheet_alloc(SHTCTL *ctl);
void sheet_setbuf(SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(SHEET *sht, int height);
void sheet_refresh(SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_refreshsub(SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap(SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_slide(SHEET *sht, int vx0, int vy0);
void sheet_free(SHEET *sht);
//void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
//void make_window8(unsigned char *buf, int xsize, int ysize, char *title);
//int keywinOff(SHEET *key_win,SHEET *sht_win, int cur_c, int cur_x);
//int keywinOn(SHEET *key_win,SHEET *sht_win, int cur_c);
//void change_wtitle8(SHEET *sht, char act);
/* void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);
void putfonts8_asc_sht(SHEET *sht, int x, int y, int c, int b, char *s, int l); */
/* int windowX = WINDOW_X;
int windowY = WINDOW_Y; */
/*默认窗口类型*/
static BOX ControlBox[] = { //控制台窗口
    {WINDOW_X,WINDOW_Y,WINDOW_Y + 60,WINDOW_X + 95,COL8_000000,"body"}, //主窗体
    {WINDOW_X,WINDOW_Y,WINDOW_X + 95,WINDOW_Y + 10,COL8_0078D7,"caption"}, //标题
    {-1,0,0,0,0,"EndFlag"} //结束标记
};
/* BOX ControlBox[] = { //控制台窗口
    {windowX,windowY,windowY + 60,windowX + 95,COL8_000000,"body"}, //主窗体
    {windowX,windowY,windowX + 95,windowY + 10,COL8_0078D7,"caption"}, //标题
    {-1,0,0,0,0,"EndFlag"} //结束标记
}; */

static BOX NormalBox[] = { //普通窗口
    {WINDOW_X,WINDOW_Y,WINDOW_X + WINDOW_WIDTH,WINDOW_Y + 60,COL8_FFFFFE,"body"}, //主窗体
    {WINDOW_X,WINDOW_Y,WINDOW_X + WINDOW_WIDTH,WINDOW_Y + 10,COL8_0078D7,"caption"}, //标题
    {-1,0,0,0,0,"EndFlag"} //结束标记
};

static BOX InformationBox[] = { //信息提示窗口
    {WINDOW_X,WINDOW_Y, WINDOW_X + 75,WINDOW_Y + 30,COL8_FFFFFE,"body"}, //主窗体
    {WINDOW_X,WINDOW_Y,WINDOW_X + 75,WINDOW_Y + 10,COL8_0078D7,"caption"}, //标题
    {-1,0,0,0,0,"EndFlag"} //结束标记
};

static BOX textBox[] = { //文本输入窗口
    //{WINDOW_X,WINDOW_Y, WINDOW_X + 75,WINDOW_Y + 30,COL8_FFFFFE,"body"}, //主窗体
	{WINDOW_X - 2, WINDOW_Y - 3, WINDOW_X + 75 + 1, WINDOW_Y - 3,COL8_848484,"caption"}, //标题
	{WINDOW_X,WINDOW_Y,WINDOW_X + 75,WINDOW_Y + 10,COL8_0078D7,"caption"}, //标题
	{WINDOW_X - 3, WINDOW_Y - 3, WINDOW_X - 3, WINDOW_Y + 30 + 1,COL8_848484,"caption"}, //标题
	{WINDOW_X - 3, WINDOW_Y + 30 + 2, WINDOW_X + 75 + 1, WINDOW_Y + 30 + 2,COL8_FFFFFF,"caption"}, //标题
	{WINDOW_X + 75 + 2, WINDOW_Y - 3, WINDOW_X + 75 + 2, WINDOW_Y + 30 + 2,COL8_FFFFFF,"caption"}, //标题
	{WINDOW_X - 1, WINDOW_Y - 2, WINDOW_X + 75, WINDOW_Y - 2,COL8_000000,"caption"}, //标题
	{WINDOW_X - 2, WINDOW_Y - 2, WINDOW_X - 2, WINDOW_Y + 30,COL8_000000,"caption"}, //标题
	{WINDOW_X - 2, WINDOW_Y + 30 + 1, WINDOW_X + 75, WINDOW_Y + 30 + 1,COL8_C6C6C6,"caption"}, //标题
	{WINDOW_X + 75 + 1, WINDOW_Y - 2, WINDOW_X + 75 + 1, WINDOW_Y + 30 + 1,COL8_C6C6C6,"caption"}, //标题
	//{WINDOW_X - 1, WINDOW_Y - 1, WINDOW_X + 75, WINDOW_Y + 30,c,"caption"}, //标题
    {-1,0,0,0,0,"EndFlag"} //结束标记
};

/*=====已废弃的声明=====================================================================================================*/
//char get_vram_info();
//BOOT_INFO initBootInfo(); //BOOT信息初始化函数
//SYSTEM_INFO initSystemInfo(); //系统信息初始化函数，SYSTEM_INFO结构体类型见systructual.h
//char rgbColor(int colorR,int colorG,int colorB); 
//void set_peletteX(); //画板设定函数增强版（速度慢），支持RGB绘图(暂时作废)
//void write_mem8(int addr, int data); //声明write_mem8函数，此函数使用汇编语言编写，编译时连接(已使用C语言指针代替)
//void fontDraw8(char *vram,SCREEN screen,FONT *font); //字体渲染函数，SCREEN、FONT结构体类型见systrual.h
//void boxDraw8(unsigned char *vram,int vwidth,unsigned char vcolor,int bx0,int by0,int bx1,int by1); //矩形区域绘制函数，参数依次为显存地址、宽度、颜色、四角位置
//#ifndef _ONCE_  //防止重复定义
//#define _ONCE_
//#endif 
//unsigned int memeryCheckSub(unsigned int start,unsigned int end);
/* void wait_KBC_sendready(void);
void init_keyboard(void);
void enable_mouse(void); */

// readBufferData(char *vram,SCREEN *screen,MOUSE_CURSOR *mouseCursor);
//void readBufferData(char *vram,SCREEN *screen,MOUSE_CURSOR *mouseCursor,COVER_LIST *coverList,PICTURE *picture,COVER *cover);
//void readBufferData(char *vram,SCREEN *screen,MOUSE_CURSOR *mouseCursor,COVER_LIST *coverList,PICTURE *picture,COVER *mouseCover);
