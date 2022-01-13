/**
 * ===================NNOS-interrupt==========================
 *【PIC初始化和系统中断处理相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/	
#include "./lib/nnos.h" //系统函数声明

/*设备缓冲区导入*/
//extern FIFO_BUFFER keyFIFOBuffer,mouseFIFOBuffer;  //键盘、鼠标先进先出缓冲区，键devices.c
extern FIFO_BUFFER32 *keyFIFOBuffer32,*mouseFIFOBuffer32; //键盘、鼠标32位先进先出缓冲区
extern TIMER_LIST timerList; //定时器列表
extern int keyDataBase,mouseDataBase;
//extern SHEET *sht_win;

/*PIC初始化函数*/
void initPIC(){
    io_out8(PIC0_IMR,0xff);     //主PIC禁止所有中断
    io_out8(PIC1_IMR,0xff);     //从PIC禁止所有中断

    io_out8(PIC0_ICW1,0x11);    //边沿触发模式(edge trigger mode)
    io_out8(PIC0_ICW2,0x20);    //IRQ0~7由INT20~27接收（主PIC负责处理0~7号中断信号）
    io_out8(PIC0_ICW3,1 << 2);  //PIC1由IRQ2连接(硬规定)
    io_out8(PIC0_ICW4,0x01);    //无缓冲区模式

    io_out8(PIC1_ICW1,0x11);    //边沿触发模式
    io_out8(PIC1_ICW2,0x28);    //IRQ8~15由INT28~2f接收（从PIC负责处理8~15号中断信号）
    io_out8(PIC1_ICW3,2);       //PIC1由IRQ2连接
    io_out8(PIC1_ICW4,0x01);    //无缓冲区模式

    io_out8(PIC0_IMR,0xfb);      //禁止除PIC以外的所有中断（11111011）
    io_out8(PIC1_IMR,0xff);      //禁止所有中断（11111111）
}
/*
 PIC内部寄存器通过端口号码彼此访问
 PIC写入ICW1后要紧接着写入ICW2，端口号相同也易区分   
 PIC中皆为8位寄存器
 IMR是Interrupt mask register的缩写，意为中断屏蔽寄存器
 8位分别对应8路IRQ信号。某位为1，则对应IRQ信号被屏蔽，PIC忽略该中断。（处理中屏蔽中断。屏蔽静电干扰。）
 ICW是Initial contrl word，意为初始化控制数据。word只在电脑CPU里指8位，其他地方不一定。
 ICW有4个，编号1~4。
 上述值为固定值，不可更改。否则，早期电脑会短路（+5V与GND短路，发生芯片保险丝熔断、器件冒烟）。
 ICW3,有关主从PIC连接的设定。对主PIC而言，第几号IRQ与从PIC连接用8位设定。全1则主PIC可驱动8个PIC，最大64个IRQ。
 电脑硬件上，CPU主从PIC通过IRQ2连接，故此值只能设定为00000100。对从PIC而言，连接方式用3位设定。
不同操作系统可进行自定义的只有ICW2,它决定了以哪一号中断通知CPU。（中断发生后，CPU若可处理，则命令PIC发2字节数据，
通过IN或OUT传输，有数据线连接。PIC用此信号发此2字节数据。数据格式为0xcd 0x**，此二字节作为机器语言执行。其中，
0xcd为调用BIOS时使用过的INT指令<INT 0x10>，最终编译成0xcd 0x10）
本系统以INT 0x20~0x2f 接收中断信号（0x00~0x1f不能用于IRQ，应用程序越权时，CPU用此地址自动中断保护通知）
*/

/*PS/2键盘中断处理器*/
void inthandler21(int *esp){
    int data;
    io_out8(PIC0_OCW2,0x61); //通知PIC，IRQ_01中断请求已处理完毕(IRQn->0x60+n=>OCW2，PCI将继续监视IRQ1中断是否发生)
    data = io_in8(PORT_KEYDAT); //从编号为0x60的设备读入8位信息编码，此处为键盘(0x0060)按键编码
	putFIFOBuffer32(keyFIFOBuffer32,data + keyDataBase); //将键盘码加上偏移量写入32位通用缓冲区
}

/*PS/2鼠标中断处理器*/
void inthandler2c(int *esp){
	int data;
	io_out8(PIC1_OCW2,0x64); //通知从PIC即PIC1，IRQ_12中断请求处理完毕（鼠标中断通过12号管脚,8~15在PIC1,为PIC1的第4号，从0开始编号）
	io_out8(PIC0_OCW2,0x62); //通知主PIC即PIC0,IRQ_02中断请求处理完毕（PIC1通过PIC1的2号管脚，0~7在PIC0）。主从PIC协调不能自动完成，须先后通知。
	data= io_in8(PORT_KEYDAT); 
	putFIFOBuffer32(mouseFIFOBuffer32,data + mouseDataBase);
} 

/*若不先后通知主从PIC，则主PIC会继续忽视从PIC的下一个中断请求。从PIC连接在注PIC的2号管脚上*/
void inthandler27(int *esp){ //针对PICO的不完全中断补丁
    io_out8(PIC0_OCW2,0x67);
}
/*补丁信息说明*/
//部分机器随着PIC初始化会产生一次IRQ7中断，若不对该中断处理程序执行STI（设置中断标志位），操作系统将启动失败

/*===========================================================
 * [关于PCI]
 * PIC可将8个中断信号(Interrupt request)合称为一个中断信号。
 * PIC监视输入管脚的8个中断信号，只要有一个中断信号进来，就将唯一的输出
 * 管脚信号置为ON，通知CUP。PIC信号共有15个，共两个PIC。
 * 与CPU直接相连的PIC称为主PIC（Master PIC），与主PIC相连的称为
 * 从PIC(Slave PIC)。主PIC负责处理0到7号中断信号，从PIC负责处理
 * 8到15号中断信号。从PCI的信号须经由主PIC传送给CPU。从PIC通过第2号
 * IRQ(Interrupt Request，中断请求)与主PIC相连，此为硬件配置。
 * 
 * 注意与PCI区分，Peripheral Component Interconnect(外设部件互连标准)
 * ===========================================================*/

/*PIT(可编程间隔定时器)初始化*/
void initPIT(){
	COUNT i;
	TIMER *timer;
	io_out8(PIT_CTRL,0x34);
	io_out8(PIT_CNT0,0x9c);
	io_out8(PIT_CNT0,0x2e); 
	//timerList.number = 0; //初始化定时器数量为0
	timerList.count = 0;
	timerList.next = TIMER_TIME_MAX_H; //初始化时没有运行中的定时器
	for(i = 0;i < TIMER_MAX;i++){
		timerList.timer[i].flag = TIMER_FLAG_UNUSED; //初始化定时器使用状态为未使用
	}
	timer = timerAlloc();
	timer->timeout = TIMER_TIME_MAX_H;
	timer->flag = TIMER_FLAG_USING;
	timer->next = 0;
	timerList.timers = timer;
	timerList.next = TIMER_TIME_MAX_H;
}

void inthandler20(int *esp){ //可改进为先排序，再依次出队，设定前刷新队列
	TIMER *timer;
	char taskFlag = 0;
	io_out8(PIC0_OCW2,0x60); //将IRQ_00信号接收完毕的信息通知PIC
	timerList.count++; //定时器开始计时（中断间隔为1秒，故定时器每秒钟增加100）
	if(timerList.next > timerList.count){
		return;
	}
	timer = timerList.timers;
	//while(timer->next != 0 && timer->next->timeout > timerList.count){
	while(timer->timeout <= timerList.count){
		timer->flag = TIMER_FLAG_USED;
		if(timer != taskTimer){
			putFIFOBuffer32(timer->fifo,timer->data);
		}
		else{
			taskFlag = 1; //中断处理未完成，不能处理新中断，须以ts作为标志切换多任务
		}
		
		timer = timer->next;
	}
	timerList.timers=timer;
	timerList.next = timer->timeout;
	if(taskFlag != 0){
		taskSwitch();
	}
	//return;
}

//=====异常中断处理器===========================================================
int *inthandler0d(int *esp){ //一般异常保护
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_0D]G-Protected Exception.\n");
	sysprintl(console,"info:Intend to run undelegated instruction.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler0c(int*esp){ //栈异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_0C]Stack Exception.\n");
	sysprintl(console,"info:App stack overflow.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler00(int*esp){ //栈异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_00]Division Exception.\n");
	sysprintl(console,"info:Division by zero.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler01(int*esp){ //栈异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_01]Debug exception.\n");
	sysprintl(console,"info:System bug.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler03(int*esp){ //栈异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_03]Single byte trap.\n");
	sysprintl(console,"info:INT 3.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler04(int*esp){ //栈异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_04]Overflow trap.\n");
	sysprintl(console,"info:INTO.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler0a(int*esp){ //无效TSS异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_0A]Invalid TSS exception.\n");
	sysprintl(console,"info:JMP/CALL/IRET/INT.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler0b(int*esp){ //段不存在异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_0A]Segment exception.\n");
	sysprintl(console,"info:The segment does not exist.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler05(int*esp){ //段不存在异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_0A]Boundary check exception.\n");
	sysprintl(console,"info:BOUNT.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}

int *inthandler08(int*esp){ //段不存在异常中断处理
	char info[32];
	TASK *task = taskNow(); //当前任务
	//CONSOLE *console = (CONSOLE *)*((int*)0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	sprintf(info,"    EIP = %08X",esp[11]); //输出信息
	sysprintl(console,"\nExc :[INT_0A]Double fault exception.\n");
	sysprintl(console,"info:None.\n");
	sysprintl(console,info);
	return &(task->tss.esp0); //强制结束应用程序
}
