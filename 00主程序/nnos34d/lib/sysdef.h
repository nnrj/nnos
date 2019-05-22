/**
 * ===================NNOS-sysdef.h===========================
 *【系统预定义常量和静态数据信息】
 * -----------------------------------------------------------
 * ·日期：2019-3-23      作者：年年软件
 * ===========================================================
**/
//#pragma once
/*版本号*/
#define NNOS_VERSION "NNOS 0.34d"

/*系统信息*/
#define NNOS_FILE_SYSTEM "FAT12"
#define NNOS_CPU_BASED "Intel_80x86"
#define NNOS_COMPANY "NNRJ"
#define NNOS_AUTHOR "Liu Dongxu"
#define NNOS_EMAIL "tianhehechu@qq.com"
#define NNOS_UPDATE_DATE "2019-5-1"

/*BOOT信息首地址*/
#define BOOTINFO_ADR 0x00000ff0

/*GDT&IDT地址*/
#define GDT_ADR 0x00270000		//GDT基址
#define IDT_ADR 0x0026f800		//IDT基址
#define GDT_LIMIT 0x0000ffff	//GDT上限
#define IDT_LIMIT 0x000007ff	//IDT上限
#define BOOT_ADR 0x00280000		//BOOT首地址
#define BOOT_LIMIT 0X0007ffff	//BOOT上限
#define DATE32_RW_PRE 0x4092	//读写权限，应用模式（用户模式，用户态）
#define CODE32_ER_PRE 0x409a	//运行权限，系统模式（内核模式，内核态）
#define INTGATE32_PRE 0x008e	//允许中断
#define GDT_SELECTOR_MAX 8192	//GDT选择子最大值
#define TSS32_PRE 0x0089		//多任务
#define LDT_PRE 0x0082			//LDT段访问权限

/*显卡常量*/
//#define VWIDTH 320	//显卡横线分辨率
//#define VHEIGHT 200 //显卡纵向分辨率
#define COLORNUM 27 //颜色库数目

/*PIC常量*/
#define PIC0_ICW1 0x0020
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1

/*PIT(可编程间隔定时器)常量*/
#define PIT_CTRL 0x0043	//PIT端口号
#define PIT_CNT0 0x0040 	//PIT端口号
#define TIMER_MAX 500 		//定时器最大数量
#define TIMER_FLAG_UNUSED 0 //定时器未使用
#define TIMER_FLAG_USED 1 //定时器已使用（启用，就绪）
#define TIMER_FLAG_USING 2//定时器正在使用
#define TIMER_TIMER_MAX 42949673 //定时器溢出上限（十进制）
#define TIMER_TIME_MAX_H 0xffffffff //定时器溢出上限（十六进制）

/*内存描述常量*/
#define MEMERY_ADDR 0x003c0000		 //内存首地址
#define MEMERY_LIST_SIZE 4096		 //内存空闲块表大小（块）
#define SYS_PRE 0x00400000  		 //系统预留内存，4MB
#define MEMERY_MAX_SIZE 0x07c00000   //支持的最大内存容量(去除预留)

/*内存处理常量*/
#define EFLAGS_AC_BITS 0x00040000 		//CPU类型校验码（486及以上CPU的EFLAGS寄存器有AC位）
#define CR0_CACHE_DISABLE 0x60000000	//禁用Cache的操作码
//#define BLOCK_ALIGN 0xfffff000		//内存块对齐操作数(4K=0x1000)

/*文件信息*/
#define DISK_ADR 0x00100000 //磁盘首地址
#define FILE_INFO_MAX 224		//文件信息最大个数（受FAT12文件系统限制）
#define FILE_NAME_SIZE	8
#define FILE_EXTEN_SIZE 3	
#define FILE_FULL_NAME_SIZE FILE_NAME_SIZE+FILE_EXTEN_SIZE

/*鼠标常量*/
#define KEYCMD_MOUSE 0xd4			//键盘控制电路切换鼠标模式指令
#define MOUSECMD_ENABLE 0xf4		//鼠标激活指令
#define ACK 0xfa 					//鼠标激活成功回答确认信息
#define MOUSE_DATA_BASE 512			//鼠标中断信号偏移

/*鼠标指针常量*/
#define CURSOR_WIDTH 16            //鼠标指针宽度
#define CURSOR_HEIGHT 16           //鼠标指针高度
#define CURSOR_PIX 16              //鼠标指针分辨率
#define CURSOR_DATA_SIZE 3		   //鼠标指针数据大小
#define CURSOR_BCOLOR DESK_BCOLOR  //鼠标指针背景色（一般设为桌面背景色）
#define CURSOR_GRAPH_SIZE (CURSOR_WIDTH * CURSOR_HEIGHT) //鼠标指针位数

/*键盘常量*/
#define PORT_KEYDAT 0x0060			//键盘设备端口号
#define PORT_KEYSTA 0x0064			//键盘状态端口号
#define PORT_KEYCMD 0x0064			//键盘控制电路端口号
#define KEYSTA_NOTREADY 0x02		//键盘未准备好代码（准备好后，在键盘0x0064处读取的数据，从低位开始数第二位，即倒数第二位须为0）
#define KEYCMD_WRITE_MODE 0x60		//键盘控制电路模式设置指令
#define KBC_MODE_MOUSE 0x47			//键盘控制电路模式之鼠标模式
#define KEY_DATA_BASE 256			//键盘中断信号偏移
#define KEYCMD_LED_STATUS 0xed		//键盘初始化操作命令(硬件规定)
#define KEY_TABLE_SIZE	0x80		//键盘字符映射表大小

/*缓冲区常量*/
#define FLAGS_OVERRUN 0x0001;	//溢出
#define KEY_BUFFER_SIZE 32		//键盘缓冲区大小
#define MOUSE_BUFFER_SIZE 128	//鼠标缓冲区大小
#define TIMER_BUFFER_SIZE 8	 	//定时器缓冲区大小
#define BUFFER_SIZE 128			//通用缓冲区大小

/*桌面常量*/
//#define DESK_WIDTH VWIDTH        	//桌面默认宽度
//#define DESK_HEIGHT VHEIGHT      	//桌面默认高度
//#define TASKBAR_WIDTH DESKWIDTH    //任务栏默认宽度
//#define TASKBAR_HEIGHT DESKHEIGHT*0.05  //任务栏默认高度
#define DESK_BCOLOR COL8_004276			//桌面默认背景色

/*图层常量*/
#define COVER_LIST_SIZE 256	//图层表最大图层数
#define COVER_UNUSE 0		//图层未使用
#define COVER_USEED	1		//图层已使用
#define COVER_NUM 15		//图层最大图形数

/*任务常量*/
#define TASK_MAX 1000		//最大任务数
#define TASK_GDT0 3			//TSS的GDT起始号码
#define TASK_FLAG_UNUSED 0 	//任务项未使用
#define TASK_FLAG_USED 1 	//任务项已使用（启用，就绪）
#define TASK_FLAG_RUNNING 2	//任务正在运行
#define TASK_LEVEL_MAX 100 	//任务优先级队列最大任务数
#define LEVEL_MAX 10		//优先级队列最大数量

/*系统默认窗口定义*/
#define WINDOW_X 5 //默认窗口横坐标
#define WINDOW_Y 5 //默认窗口纵坐标
#define WINDOW_WIDTH  70 //默认窗口宽度
#define WINDOW_HEIGHT 50 //默认窗口高度
#define WINDOW_LINE_WIDTH 1 //默认窗口边线宽度
#define WINDOW_CAPTION_HEIGHT 20 //默认窗口标题高度
#define WINDOW_LINE_COLOR COL8_101010 //默认窗口边线颜色（获得焦点时）
#define CONTROL_WINDOW 1 //控制台窗口类型号
#define NOMAL_WINDOW 2 //通用窗口类型号
#define INFO_WINDOW 3 //信息提示窗口类型号
#define TEXT_WINDOW 4 //文本输入窗口类型号
#define CONSOLE_WIDTH 400 //默认控制台宽度
#define CONSOLE_HEIGHT 250 //默认控制台高度
//#define CONSOLE_TEXT_WIDTH CONSOLE_WIDTH - 16 //默认控制台文本输入区域宽度
//#define CONSOLE_TEXT_HEIGHT CONSOLE_HEIGHT - 17 //默认控制台文本输入区域高度
#define CONSOLE_FORECOLOR COL8_38CE2F //默认控制台前景色
#define REFRESH_ALL 0 //窗口区域全部刷新操作数
#define REFRESH_CAPTION 1 //窗口标题局部刷新操作数
#define CMD_CURSOR_X 16 //默认控制台光标初始横坐标
//#define CMD_CURSOR_Y 60 //默认控制台光标初始纵坐标
#define CMD_CURSOR_Y 28 //默认控制台光标初始纵坐标


/*颜色序号*/
#define COL8_000000 0   //纯黑
#define COL8_FF0000 1   //纯红
#define COL8_00FF00 2   //纯绿
#define COL8_0000FF 3   //纯蓝
#define COL8_FFFF00 4   //纯黄
#define COL8_FF00FF 5   //纯紫
#define COL8_00FFFF 6   //纯青
#define COL8_FFFFFF 7   //纯白
#define COL8_C6C6C6 8   //纯灰
#define COL8_840000 9   //暗红
#define COL8_008400 10  //暗绿
#define COL8_000084 11  //靛青
#define COL8_848400 12  //暗黄
#define COL8_840084 13  //暗紫
#define COL8_008484 14  //靛蓝
#define COL8_848484 15  //暗灰
#define COL8_005B9E 16  //湛蓝
#define COL8_0078D7 17  //浅蓝
#define COL8_004276 18  //深蓝
#define COL8_FFFFFE 19  //墨白
#define COL8_E1E1E1 20  //浅灰
#define COL8_101010 21  //明黑
#define COL8_333333 22  //黑灰
#define COL8_D9D9D9 23	//银灰
#define COL8_E81123 24  //亮红
#define COL8_F0F0F0 25  //薄灰
#define COL8_38CE2F 26	//亮青

/*常用颜色别名*/
#define FRESH_BLUE 16   //湛蓝
#define TEEN_BLUE 17    //浅蓝
#define DEEP_BLUE 18    //深蓝
#define FLOUR_WHITE 19  //墨白
#define REAL_RED 1      //纯红
#define REAL_YELLOW 4   //纯黄
#define REAL_BLUE 3     //纯蓝
#define REAL_GREEN 2    //纯绿

/*字体常量*/
#define FONT_SIZE 16    //字符位数常量
#define FONT_CHAR 1     //字符字节数常量

/*其他公用常量*/
#define ZERO_ADR 0x00000000 //特殊地址_零地址
#define FULL_ADR 0xffffffff //特殊地址_最大地址
//#define NULL 0
#define TRUE 1
#define FALSE 0
//#define NONE -1
#define NONE 0

/*第三方临时常量)*/
#define MEMMAN_FREES 4090
#define MEMMAN_ADDR	 0x003c0000
#define MAX_SHEETS	 256
#define APP_FLAG	 "Hari"

/*专用类型定义*/
typedef int COUNT;				//定义COUNT类型（实质为int的别名，计数时使用，使之易于理解）

/*系统颜色信息*/ //使用静态一维数组存储数量有限的常用颜色，使用此函数绘图速度快
static unsigned char table_rgb[COLORNUM * 3] = { //静态无符号的16×3的颜色表格二维数组（节省空间，亦可加快绘制速度）。static char 只能用于数据
    0x00,0x00,0x00,	//纯黑  0                   //颜色值定义为无符号数，防止意外溢出后由亮色变为暗色
    0xff,0x00,0x00, //纯红  1
    0x00,0xff,0x00, //纯绿  2
    0x00,0x00,0xff, //纯蓝  3
    0xff,0xff,0x00, //纯黄  4
    0xff,0x00,0xff, //纯紫  5
    0x00,0xff,0xff,	//纯青  6
    0xff,0xff,0xff, //纯白  7
    0xc6,0xc6,0xc6, //纯灰  8
    0x84,0x00,0x00, //暗红  9
    0x00,0x84,0x00, //暗绿  10
    0x00,0x00,0x84, //靛青  11
    0x84,0x84,0x00, //暗黄  12
    0x84,0x00,0x84, //暗紫  13
    0x00,0x84,0x84, //靛蓝  14
    0x84,0x84,0x84,	//暗灰  15
    0x00,0x5b,0x9e, //湛蓝  16
    0x00,0x78,0xd7, //浅蓝  17
    0x00,0x42,0x76, //深蓝  18
    0xff,0xff,0xfe, //墨白  19
    0xe1,0xe1,0xe1,	//浅灰	20
    0x10,0x10,0x10, //明黑  21
    0x33,0x33,0x33, //黑灰  22
	0xd9,0xd9,0xd9, //银灰  23
	0xe8,0x11,0x23,	//亮红	24
	0xf0,0xf0,0xf0, //薄灰  25
	0x38,0xce,0x2f, //亮青	26
    //浅黑 4c4a48
};

/*鼠标指针点阵*/
static char CUSOR_GRAPH[CURSOR_WIDTH][CURSOR_HEIGHT] = { //使用16*16点阵画出鼠标指针，点阵数据存在二维数组中
    "**00000000000000",
    "*1*0000000000000",
    "*111*00000000000",
    "*11111*000000000",
    "*1111111*0000000",
    "*111111111*00000",
    "*111111111110000",
    "*111111111111**0",
    "*1111111111111*",
    "*11111111*******",
    "*111111111*00000",
    "*1111*11111*0000",
    "*111*0*11111*000",
    "*11*000*1111*000",
    "*1*00000*111*000",
    "**0000000****000"
};

/* static char KEY_TABLE[0x54] = {
	'\0','\1','1','2','3','4','5','6','7','8','9','0','-','=','\e','\f',
	'Q','W','E','R','T','Y','U','I','O','P','[',']','\1c','\1d',
	'A','S','D','F','G','H','J','K','L',';','\'','`','\2a','/',
	'Z','X','C','V','B','N','M',',','.','/','\36','*','\38',' ',
	'\3a','\3b','\3c','\3d','\3e','\3f','\40','\41','\42','\43','\44','\45','\46',
	'7','8','9','_','4','5','6','+','1','2','3','0','.'
}; */
/* static char KEY_TABLE[0x54] = {
	0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','[',']',0,0,
	'A','S','D','F','G','H','J','K','L',';','\'','`',0,'/',
	'Z','X','C','V','B','N','M',',','.','/',0,'*',0,' ',
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	'7','8','9','-','4','5','6','+','1','2','3','0','.'
};

static char KEY_TABLE_CTRL[0x54] = {
	0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,
	'A','S','D','F','G','H','J','K','L',':','\"','`',0,'/',
	'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	'7','8','9','-','4','5','6','+','1','2','3','0','.'
}; */

static char KEY_TABLE[KEY_TABLE_SIZE] = {
	0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','[',']',0,0,
	'A','S','D','F','G','H','J','K','L',';','\'','`',0,'/',
	'Z','X','C','V','B','N','M',',','.','/',0,'*',0,' ',
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	'7','8','9','-','4','5','6','+','1','2','3','0','.',
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0x5c,0, 0,0,0,0,0,0,0,0,0x5c,0,0
};

static char KEY_TABLE_CTRL[KEY_TABLE_SIZE] = {
	0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,
	'A','S','D','F','G','H','J','K','L',':','\"','`',0,'/',
	'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	'7','8','9','-','4','5','6','+','1','2','3','0','.',
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,'_',0, 0,0,0,0,0,0,0,0,'|',0,0
};

/* static char KEY_TABLE[0x80] = {
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
};
static char KEY_TABLE_CTRL[0x80] = {
	0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
}; */

/* static char KEY_TABLE_CTRL[0x80] = {
	0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,
	'A','S','D','F','G','H','J','K','L',':','\"','`',0,'/',
	'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' ',
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	'7','8','9','-','4','5','6','+','1','2','3','0','.',
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,'_',0,0,0,0,0,0,0,0,0,'|',0,0
};
 */
/* 	static char keytable0[0x80] = {
		0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0
	};
	static char keytable1[0x80] = {
		0,   0,   '!', 0x22, '#', '$', '%', '&', 0x27, '(', ')', '~', '=', '~', 0,   0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '`', '{', 0,   0,   'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', '+', '*', 0,   0,   '}', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   '_', 0,   0,   0,   0,   0,   0,   0,   0,   0,   '|', 0,   0
	}; */

/*=====已废弃的常量====================================================================================*/

//#define VRAM_MODE01 

/*=====系统公共变量======*/
/*键盘缓冲区*/
//KEY_BUFFER keyBuffer;

/*鼠标指针点阵2（暂时不用）*/
/* static char cursor2[CURSOR_WIDTH][CURSOR_HEIGHT] = { //使用16*16点阵画出鼠标指针，点阵数据存在二维数组中
    "**************00",
    "*11111111111*000",
    "*1111111111*0000",
    "*111111111*00000",
    "*11111111*000000",
    "*1111111*0000000",
    "*1111111*0000000",
    "*11111111*000000",
    "*1111**111*00000",
    "*111*00*111*0000",
    "*11*0000*111*000",
    "*1*000000*111*00",
    "**00000000*111*0",
    "*0000000000*111*",
    "000000000000*11*",
    "0000000000000***"
}; */

/*typedef COL8_9E5B00 FRESH_BLUE;
typedef COL8_d77800 TEEN_BLUE;
typedef COL8_764200 DEEP_BLUE;
typedef COL8_FFFFFE FLOUR_WHITE;
typedef COL8_FF0000 REAL_RED;
typedef COL8_FFFF00 REAL_YELLOW;
typedef COL8_0000FF REAL_BLUE;*/

/*关闭按钮点阵*/
/* static char CLOSE_BUTTON[14][16] = {
	"OOOOOOOOOOOOOOO@",
	"OQQQQQQQQQQQQQ$@",
	"OQQQQQQQQQQQQQ$@",
	"OQQQ@@QQQQ@@QQ$@",
	"OQQQQ@@QQ@@QQQ$@",
	"OQQQQQ@@@@QQQQ$@",
	"OQQQQQQ@@QQQQQ$@",
	"OQQQQQ@@@@QQQQ$@",
	"OQQQQ@@QQ@@QQQ$@",
	"OQQQ@@QQQQ@@QQ$@",
	"OQQQQQQQQQQQQQ$@",
	"OQQQQQQQQQQQQQ$@",
	"O$$$$$$$$$$$$$$@",
	"@@@@@@@@@@@@@@@@"	
}; */

