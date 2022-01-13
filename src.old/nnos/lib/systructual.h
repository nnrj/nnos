/**
 * ===================NNOS-systructual.h======================
 *【系统结构体类型相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
//=====全局描述符(GDT)===================================================
typedef struct SEGMENT_DESCRIPTOR{  //以CPU信息为基础的结构体，存放8字节内容（CPU规格要求，段信息按8字节写入内存）
    short LOW_LIMIT;                //低位上限
    short LOW_BASE;                 //低位基址（低位2字节基地址）
    char MID_BASE;                  //中位基址（中位1字节基地址）
    char ACCESS_PER;                //操作权限（禁止写入、禁止执行、系统专用）（0x9a，系统模式；0x92，应用模式）
    char HIGH_LIMIT;                //高位上限
    char HIGH_BASE;                 //高位基址（高位1字节基地址，低、中、高三个基地址共32位,分三段，可兼容80286）
}SEGMENT_DESCRIPTOR;
/*GDT备注*/
/*
1.段上限最大为4GB，为32位保护模式可访问的最大内存。段本身4字节，4字节，共需8字节，因此段上限只能用20位，最大1MB。Gbit=1,32位模式，Gbit=0,16位模式；
2.段中设有标志位Gbit，标志位为1时limit单位不解释为byte,而解释为页，1page=4KB。4KB*1M = 4GB。Gbit=granularity,单位大小；
3.ACCESS_PER为访问权限，共12位，高4位放在HIGH_LIMT中，xxxx0000xxxxxxxx.高4位为扩展访问权，386以后才使用由GD00构成，Gbit，段模式。
4.ACCESS_PER低8位构成（80386前便有）
    00000000(0x00)：未使用的记录（descriptor table）
    10010010(0x92)：系统专用，可读可写段，不可执行。
    10011010(0x9a)：系统专用，可执行段，可读不可写。
    11110010(0xf2)：应用程序用，可读写段，不可执行。
    11111010(0xfa)：应用程序用，可执行段，可读不可写。
5.32位模式下，CUP有系统模式(内核模式，ring0，环节0)和应用模式(用户模式,ring3)之分。此外，ring1、ring2由设备驱动器(device driver)等使用。
*/

//=====中断描述符(IDT)==================================================
typedef struct GATE_DESCRIPTOR{     //以CPU信息为基础的结构体，存放8字节内容
    short LOW_OFFSET;  				//低位偏移
	short SELECTOR;                 //段选择子（段号）
    char DW_COUNT;					//计数器
	char ACCESS_PER; 				//段访问权限
    short HIGH_OFFSET;				//高位偏移
}GATE_DESCRIPTOR;

//=====任务状态段(TSS)==================================================
/*TSS结构体--------*/
typedef struct TSS32{ //26个int成员,104字节
	int backlink,esp0,ss0,esp1,ss1,esp2,ss2,cr3;  //与任务设置有关的信息，可暂时忽略
	int eip,eflags,eax,ecx,edx,ebx,esp,ebp,esi,edi; //32位寄存器
	int es,cs,ss,ds,fs,gs; //16位寄存器
	int ldtr,iomap; //任务设置相关，任务奇幻史不会被CPUT写入，不可忽略，必须按规定赋值
}TSS32;

//=====缓冲区==========================================================
struct TASK;
/*鼠标键盘专用缓冲区*/
typedef struct FIFO_BUFFER{
	unsigned char *buffer; //缓冲区首地址
	int next_write; //写指针
	int next_read; //读指针
	int size; //总容量
	int free; //空闲容量
	int flags; //标志位
}FIFO_BUFFER;

/*32位通用缓冲区*/
typedef struct FIFO_BUFFER32{
	int *buffer; //缓冲区首地址
	int next_write; //写指针
	int next_read; //读指针
	int size; //总容量
	int free; //空闲容量
	int flags; //标志位
	struct TASK *task; //任务指针
}FIFO_BUFFER32;


/*任务结构体*/
typedef struct TASK{
	int selector; //段选择子(段选择符，段地址标识，段编号)
	int flag; //状态标志
	TSS32 tss; //任务状态段
	int priority; //优先级
	int level; //优先级队列
	struct FIFO_BUFFER32 fifo; //32位通用缓冲区
	struct CONSOLE *console; //控制台指针
	int base; //数据段首地址
	int consoleStack; //控制台地址栈，用于记录打开的控制台的地址，关闭时使用
	struct SEGMENT_DESCRIPTOR ldt[2]; //LDT段
	struct FILE_HANDLE *file_handle; //文件处理器
	int *fat; //文件分配表指针
	char *command;
	char lang_mode; //语言模式
	int lang_byte; //接收到全角时存放第一个字节内容，接收到半角或全角完成后此变量被置为0
}TASK;

/*任务优先级队列*/
typedef struct TASK_LEVEL{
	int number; //运行中的任务数量
	int now; //当前正在运行的任务
	TASK *tasks[TASK_LEVEL_MAX]; //优先级队列任务列表
}TASK_LEVEL;

/*任务列表结构体*/
typedef struct TASK_LIST{
	int level_run; //运行中的任务数量
	char level_change;
	//int now; //当前正在运行的任务
	TASK_LEVEL task_level[LEVEL_MAX]; //任务栏运行队列
	TASK tasks0[TASK_MAX]; //任务列表
}TASK_LIST;

/*文件处理器*/
typedef struct FILE_HANDLE{
	char *buffer;
	int size;
	int pos;
}FILE_HANDLE;

//=====系统硬件描述=====================================================
/*BOOT信息结构体*/           //信息数据来自于syshead.asm
 typedef struct BOOT_INFO{   //指针指向相应信息
    char cyls_max;         //磁盘最大装载柱面号（启动区读硬盘读到何处为止）
    char leds_status;      //键盘LED指示灯状态
    char vram_mode;         //显卡模式（位数，颜色模式）
	char reserve;			//保留区域
    short screen_width;    //显示器行分辨率（单位：像素）
    short screen_height;   //显示器列分辨率（单位：像素）   
    char *vram_base;         //显存首地址 
}BOOT_INFO; 

/*屏幕结构体*/
typedef struct SCREEN{  //【害群之马，永不复用】
	unsigned char *vram;
    short scrx;
    short scry;
	int centerX;
	int centerY;
}SCREEN;

/*鼠标指针结构体*/  
typedef struct MOUSE_CURSOR{
	unsigned char dataBuffer[CURSOR_DATA_SIZE];
	unsigned phase;
	int mx;
	int my;
	int mbutton;
	char cursor_graph[CURSOR_GRAPH_SIZE];
}MOUSE_CURSOR;

/*内存空闲块结构体*/
typedef struct FREE_BLOCK{ //空闲块
	unsigned int addr;
	unsigned int size;
}FREE_BLOCK;

/*内存空闲块表结构体*/
typedef struct MEMERY_LIST{ //空闲块表
	int number;	//空闲块数量
	int max_number;	//最大空闲内存块数量
	int lost_number; //释放内存失败的内存块数量
	int lost_size;	//释放内存失败的次数
	unsigned int total_size;
	unsigned int total_free_size; //空闲内存总大小
	FREE_BLOCK free[MEMERY_LIST_SIZE]; //内存表主体
}MEMERY_LIST;

/*定时器结构体*/
typedef struct TIMER{
	struct TIMER *next;
	unsigned int timeout; //超时时间
	int data; //超时信息
	FIFO_BUFFER32 *fifo; //信息缓冲区
	unsigned int flag; //状态信息(未使用、已设置<使用>、运行中)
	unsigned int flag2; //自动关闭标志，是否在程序结束后自动关闭
}TIMER;

/*定时器列表结构体*/
typedef struct TIMER_LIST{
	unsigned int count;	//计时器
	unsigned int next; //下一个即将超时的时刻（可改进为下一个计时器编号）
	TIMER *timers; //运行中的定时器链表
	TIMER timer[TIMER_MAX]; //定时器数组
}TIMER_LIST;


//=====绘图结构体描述=====================================================
/*像素点结构体*/
typedef struct PIX_POINT{
    int px;                 //点横坐标
    int py;                 //点纵坐标
    unsigned char vcolor;   //填充色
}PIX_POINT;

/*色块结构体*/
typedef struct BOX{
    int bx0;                //矩形横向起点
    int by0;                //矩形纵向起点
    int bx1;                //矩形横向终点	
    int by1;                //矩形纵向终点
    unsigned char vcolor;   //填充色
    char box_name[8];
}BOX;

/*图形结构体*/
typedef struct PICTURE{
    int px;              //横坐标
    int py;              //纵坐标
    int width;          //图形宽度
    int height;         //图形高度
    //int size;           //每行像素数
    char *base;         //图形内存基址
}PICTURE;

/*窗口结构体*/
typedef struct WINDOW{
    int type; //窗口类型（1-普通，2-提示，3-询问，4全屏）
    int X;
    int Y;	
    int width; 
    int height;
    char *caption;
    unsigned char foreColor;
    unsigned char backColor;
    int visible;
    int lock;
}WINDOW;

typedef struct WINDOW_LIST{
	int number;
	int now;
	WINDOW autoWindow[100];
}WINDOW_LIST;

/*图层结构体*/
typedef struct COVER{
	int id; //图层ID
	PICTURE picture;  //图层图形数组（宽、高、横纵坐标、图形首地址）
	int order; //图层层级
	int flag; //使用状态
	int trans_color; //透明色号
	struct COVER *next;
}COVER;

/*图层列表结构体*/
typedef struct COVER_LIST{
	SCREEN screen;	//屏幕信息（宽、高、显存首地址）
	//COVER covers[COVER_LIST_SIZE]; //图层数组
	COVER *head; //图层链表首指针
	COVER *rear; //图层链表尾指针
	int top; //当前最高图层层次
	int id_max; //当前最大id
}COVER_LIST;


/*第三方临时结构体*/
typedef struct SHEET{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
	TASK *task;
}SHEET;
typedef struct SHTCTL{
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
}SHTCTL;

//=====系统软件描述=======================================================
/*文件信息*/
typedef struct FILE_INFO{ //根据nnos.img二进制文件0x002600字节之后内容创建
	unsigned char name[8];	//文件名,字符编码信息无负数
	unsigned char exten[3]; //拓展名
	unsigned char type;		//文件类型（常为0x00或0x20），0x01 只读，0x02 隐藏，0x04 系统文件，0x08 非文件信息（磁盘名称等），0x10 目录，多个属性只需相加
	char reserve[10];		//保留区域，FAT12格式保留区域，微软规定，自行定义易发生兼容问题
	unsigned short time;	//修改时间，须公式转换，下同
	unsigned short date;	//修改日期
	unsigned short cluster;	//簇号，软盘每簇为1个扇区。此处标识文件从哪个扇区开始存放，小端模式
	unsigned int size;		//文件大小
}FILE_INFO;

/*控制台*/
typedef struct CONSOLE{
	SHEET *sheet;	 //控制台图层指针
	int cursorX; //光标横坐标
	int cursorY; //光标纵坐标
	int cursorC; //光标颜色
	TIMER *timer;
}CONSOLE;

/*
若文件名第一个字节为0xe5，代表此文件已被删除
第一个字节为0x00，代表此段不任何文件信息
从磁盘镜像0x004200开始存放nnos.sys，文件信息最多存放224个
文件名不够8字节或无拓展名皆用空格补足，皆为大写字母
文件属性：
	0x01 只读
	0x02 隐藏
	0x04 系统文件
	0x08 非文件信息（磁盘名称等）
	0x10 目录
多个属性只需相加，只读+隐藏=0x01+0x03 = 0x04
*/
