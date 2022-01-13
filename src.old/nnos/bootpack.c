/**
*===========================================================
*					  系统主程序
*======================NNOS-34d=============================
*#新增设备管理器（图形化，未完成）
*#新增文件浏览器（图形化，未完成）
*#新增生命游戏（移植未完成）
*#支持文件写入（未完成）
*-----------------------------------------------------------
* ·日期：2019-5-1~5      作者：年年软件
*===========================================================
*[备注]1*8 OS Data;2*8 OS Code;1003*8 AppCode;1004*8 AppDate
*===========================================================
**/

/*头文件引用*/					 //0x280000~0x2ffffff为*.h
#include "./lib/nnos.h"			//系统函数声明（内部+外部）

/*引用外部变量*/
//extern TIMER_LIST timerList;
int CX = 40; int CY = 120;

void NNOSMain(){ //不能用return
	BOOT_INFO *bootInfo = (BOOT_INFO *) BOOTINFO_ADR; //设置BOOT信息存储位置
	FIFO_BUFFER32 FIFOBuffer32,FIFOKeyCMD;
	char info[40]; //信息
	int buffer[128],keyCMDBuffer[32];
	int vmx, vmy, data;//屏幕宽度、屏幕高度、数据信息
	MOUSE_CURSOR mouseCursor; //鼠标指针
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	SHTCTL *shtctl; //图层列表
	unsigned char *buf_back, buf_mouse[256]; //桌面背景、鼠标指针、窗口信息数组
	SHEET *sht_back,*sht_mouse;//图层
	TASK *taskA,*task;
	TIMER *timer2,*timer3; //定时器
	int centerX = (bootInfo->screen_width - 16) / 2; //屏幕中心点横坐标
	int centerY = (bootInfo->screen_height - 28 - 16) / 2; //屏幕中心点纵坐标
	vmx = 0;vmy = 0;
	WINDOW memeryInfoWindow = {INFO_WINDOW,50,5,120,55,"Memery Check",COL8_000000,COL8_FFFFFE,1,1}; //内存信息窗口
	WINDOW deviceInfoWindow = {INFO_WINDOW,180,10,200,75,"Device Code",COL8_000000,COL8_FFFFFE,1,1}; //设备信息窗口
	int keyShift = 0;
	int keyLEDS = (bootInfo->leds_status >> 4) & 7; //取出键盘LED灯状态信息的第4~6位
	int keyCMDWait = -1; //等待键盘控制电路操作命令执行完毕
	int j,x,y,mmx = -1,mmy = -1;
	SHEET *sht = 0, *keyWin;
	int rightFlag = 1;
	
	int *fat;
	unsigned char *fonts;
	FILE_INFO *fileInfo;
	extern char fonbase[4096]; //引入基本ASCII字库

	initGdtIdt();	//初始化GDT（全局描述符）和IDT（中断描述符）
	initPIC();		//初始化PIC(可编程中断处理器)
	io_sti(); //允许中断发生（初始化PIC时禁止了中断发生），函数见osfun.asm
	initFIFOBuffer32(&FIFOBuffer32,buffer,128,0); //初始化32位通用缓冲区
	*((int *) 0x0fec) = (int)&FIFOBuffer32; //存储缓冲区首地址
	initPIT();		//初始化PIT(可编程间隔定时器)
	initKeyboardCMD32(&FIFOBuffer32,256); //初始化键盘控制电路，32位，信号偏移KEY_DATA_BASE = 256
	enableMouse32(&FIFOBuffer32,512,&mouseCursor); //激活鼠标，32位，信号偏移MOUSE_DATA_BASE = 512
	//io_out8(PIC0_IMR,0xf9); //允许PIC1接收键盘中断（11111001），与CLI互逆，执行STI后，IF(interrupt flag，中断标志许可位)为1，接受外部设备中断	
	io_out8(PIC0_IMR,0xf8); //允许PIT、PIC1和键盘中断（11111000），与CLI互逆，执行STI后，IF(interrupt flag，中断标志许可位)为1，接受外部设备中断
	io_out8(PIC1_IMR,0xef); //允许接收鼠标中断（(11101111)
	initFIFOBuffer32(&FIFOKeyCMD,keyCMDBuffer,32,0); //初始化键盘操作命令缓冲区

	initMemeryManager(memeryList);	//初始化内存管理系统 
	memeryFree(memeryList,0x00001000,0x0009e000); //设置空闲内存,0x00001000 - 0x0009efff
	memeryFree(memeryList,0x00400000,memeryList->total_size - 0x00400000); //设置空闲内存

	initPalette();	//初始化画板
	shtctl = shtctl_init(memeryList,bootInfo->vram_base,bootInfo->screen_width,bootInfo->screen_height); //为图层表申请内存空间
	taskA = initTaskList(memeryList); //初始化任务列表并设置默认任务
	//taskA->priority = 10; //设置主任务为最高优先级
	FIFOBuffer32.task = taskA; //32位FIFO缓冲区任务指针指向taskA
	taskRun(taskA,1,2); //优先级队列为1，优先级为默认
	*((int *) 0x0fe4) = (int)shtctl;
	taskA->lang_mode = 0;

	/*载入日语字库*/
	/* int i = 0;
	////fonts = (unsigned char *)memeryAlloc4k(memeryList, 16 * 256 + 32 * 94 * 47);
	fonts = (unsigned char *)memeryAlloc4k(memeryList,0x5d5d * 32); //0xfefe - 0xa1a1 = 0x5d5d
	fat = (int *)memeryAlloc4k(memeryList, 4 * 2880);
	readFAT(fat,(unsigned char *)(DISK_ADR + 0x000200));
	//fileInfo = file_search2("fonts.fnt", (struct FILE_INFO *)(DISK_ADR + 0x002600),224);
	////fileInfo = file_search("fonts.fnt");
	fileInfo = file_search("HZK16.fnt");
	if (fileInfo != 0) {
		loadFile(fileInfo->cluster,fileInfo->size, fonts, fat,(char *)(DISK_ADR + 0x003e00));
	} else {
		for (i = 0; i < 16 * 256; i++) {
			fonts[i] = fonbase[i]; //半角假名
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			fonts[i] = 0xff; //无字库，全角部分以0xff填充
		}
	}
	*((int *) 0x0fe8) = (int)fonts;
	memeryFree4k(memeryList,(int) fat,4 * 2880); */
	
	/*载入字库*/
	//fonts = (unsigned char *)memeryAlloc4k(memeryList, 16 * 256 + 32 * 94 * 47);
	fonts = (unsigned char *)memeryAlloc4k(memeryList, 0x5d5d * 32);
	fat = (int *)memeryAlloc4k(memeryList, 4 * 2880);
	readFAT(fat, (unsigned char *) (DISK_ADR + 0x000200));
	fileInfo = file_search2("HZK16.fnt", (FILE_INFO *) (DISK_ADR + 0x002600), 224);
	//fileInfo = file_search("HZK16.fnt");
	if (fileInfo != 0) {
		//file_loadfile(fileInfo->cluster, fileInfo->size, fonts, fat, (char *) (DISK_ADR + 0x003e00));
		loadFile(fileInfo->cluster, fileInfo->size, fonts, fat, (char *) (DISK_ADR + 0x003e00));
	} else {
		for (data = 0; data < 16 * 256; data++) {
			fonts[data] = fonbase[data];  //半角
		}
		for (data = 16 * 256; data < 16 * 256 + 32 * 94 * 47; data++) {
			fonts[data] = 0xff;  //无字库，全角部分以0xff填充
		}
	}
	*((int *) 0x0fe8) = (int) fonts;
	memeryFree4k(memeryList, (int) fat, 4 * 2880);	
	
	
	
	/*背景相关*/
	sht_back  = sheet_alloc(shtctl); //分配背景图层
	buf_back  = (unsigned char *) memeryAlloc4k(memeryList, bootInfo->screen_width * bootInfo->screen_height); //为桌面背景图层缓冲区申请空间
	sheet_setbuf(sht_back, buf_back, bootInfo->screen_width, bootInfo->screen_height, -1);  //设置桌面背景图层
	initDesk(buf_back,bootInfo->screen_width, bootInfo->screen_height); //初始化桌面
	wordsDraw8(buf_back,bootInfo->screen_width,centerX - 30,centerY,COL8_FFFFFF,NNOS_VERSION); //字体渲染
	wordsDraw8(buf_back,bootInfo->screen_width,centerX - 30 - 1,centerY - 1,COL8_000000,NNOS_VERSION); //字体渲染
	keyWin = openConsole(shtctl);
	
	/*定时器、光标相关*/
	//cursorX = 8;cursorC = COL8_FFFFFF;
	////timer = timerAlloc();initTimer(timer,&FIFOBuffer32,1);setTimer(timer,50); //初始化定时器缓冲区
	timer2 = timerAlloc();initTimer(timer2,&FIFOBuffer32,10);setTimer(timer2,1000); //初始化定时器缓冲区
	timer3 = timerAlloc();initTimer(timer3,&FIFOBuffer32,3);setTimer(timer3,300);
 
	/*鼠标相关*/
	sht_mouse = sheet_alloc(shtctl); //鼠标图层分配
	sheet_setbuf(sht_mouse, buf_mouse, 16,16,99); //鼠标图层设定
	initMouseCursor8(buf_mouse,99); //初始化鼠标指针图像点阵
	vmx = centerX;vmy = centerY;data = -1; 

	/*图层调整*/
	sheet_slide(sht_back,0,0); //桌面背景图层复位
	//sheet_slide(keyWin,40,120);
	sheet_slide(sht_mouse,vmx,vmy); //鼠标图层复位
	sheet_updown(sht_back,0); //图层顺序设定
	//sheet_updown(keyWin,1);
	sheet_updown(sht_mouse,2); 
	//keywinOn(keyWin);	

	putFIFOBuffer32(&FIFOKeyCMD,KEYCMD_LED_STATUS); //初始化键盘操作命令缓冲区数据
	putFIFOBuffer32(&FIFOKeyCMD,keyLEDS); //将LED等状态信息写入键盘操作命令缓冲区
	
	/*内存窗口相关*/
	windowDraw8(buf_back,bootInfo->screen_width,memeryInfoWindow,0,REFRESH_ALL); //绘制内存信息窗口
	sprintf(info, "Memery:%dMB", (memeryList->total_size / (1024 * 1024)));  //输出内存总量信息
	wordsDraw8(buf_back,bootInfo->screen_width,memeryInfoWindow.X +  2,memeryInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 2,memeryInfoWindow.foreColor,info); //绘制内存总量信息
	sprintf(info,"Free:%dKB",(memeryList->total_free_size / 1024)); //输出空闲内存信息
	wordsDraw8(buf_back,bootInfo->screen_width,memeryInfoWindow.X +  2,memeryInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 18,memeryInfoWindow.foreColor,info); //绘制内存余量信息
	sheet_refresh(sht_back, memeryInfoWindow.X, memeryInfoWindow.Y,memeryInfoWindow.X + memeryInfoWindow.width + WINDOW_LINE_WIDTH,memeryInfoWindow.Y + memeryInfoWindow.height + WINDOW_LINE_WIDTH); //刷新图层列表	

	

	while(1){ //永真循环
		//myCount++; //测试用累加
		//sprintf(info,"%10d",timerList.timers->next->next->next->next->timeout); //输出测试信息
		//labelDraw(sht_win,16,28,COL8_000000,COL8_FFFFFE,info,10); //绘制标签
		if(getFIFOBufferStatus32(&FIFOKeyCMD) > 0 && keyCMDWait < 0){ //检查键盘控制命令缓冲区状态
			keyCMDWait = getFIFOBuffer32(&FIFOKeyCMD);
			waitKBCReady(); //等待键盘控制电路命令执行完毕进入稳定状态并返回信息
			io_out8(PORT_KEYDAT,keyCMDWait); //将缓冲区数据写入键盘数据端口
		}
		io_cli();
		if(getFIFOBufferStatus32(&FIFOBuffer32) == 0){ //若32位通用缓冲区为空，则开中断		
			//io_stihlt(); //先执行开中断再执行停机等待指令，但两条指令不能分开先后执行，否则其间有中断发生会导致崩溃
			taskSleep(taskA); //任务休眠
			io_sti(); //任务休眠后再开中断
			//io_sti(); //测试专用，不停机
		}
		else{ //消除外层判断（经验证不如保留，故不变）
			data = getFIFOBuffer32(&FIFOBuffer32);
			io_sti();
			if (keyWin != 0 && keyWin->flags == 0) {	//窗口关闭
				if (shtctl->top == 1) {	//最高图层为1，只剩桌面和鼠标
					keyWin = 0;
				}
				else{
					keyWin = shtctl->sheets[shtctl->top - 1];
					keywinOn(keyWin);
				}
			}			
			if(data >= 256 && data <= 511){ //键盘缓冲区非空
				sprintf(info,"KeyCode:%02X",data - 256);
				windowDraw8(buf_back,bootInfo->screen_width,deviceInfoWindow,0,REFRESH_ALL);
				wordsDraw8(buf_back,bootInfo->screen_width,deviceInfoWindow.X + 6,deviceInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 5,deviceInfoWindow.foreColor,"Type:PS/2 Keyboard");
				wordsDraw8(buf_back,bootInfo->screen_width,deviceInfoWindow.X + 6,deviceInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 25,deviceInfoWindow.foreColor,info); 
				sheet_refresh(sht_back, deviceInfoWindow.X, deviceInfoWindow.Y,deviceInfoWindow.X + deviceInfoWindow.width + WINDOW_LINE_WIDTH,deviceInfoWindow.Y + deviceInfoWindow.height + WINDOW_LINE_WIDTH);		
				if(data < 0x80 + 256 && KEY_TABLE[data - 256] != 0){
					if(keyShift == 0){ //Shift关
						info[0] = KEY_TABLE[data - 256];
					}
					else{ //Shift开
						info[0] = KEY_TABLE_CTRL[data - 256];
					}					
				}
				else{
					info[0] = 0;
				}				
				if(info[0] >= 'A' && info[0] <= 'Z'){ //输入为字幕
					if(((keyLEDS &4) == 0 && keyShift == 0) || ((keyLEDS &4) != 0 && keyShift != 0) ){ //CapsLock关，Shift关；CapsLock开，Shift开。
						info[0] += 0x20; //大写转小写
					}
				}
				if(info[0] != 0 && keyWin != 0){ //普通字符
					putFIFOBuffer32(&keyWin->task->fifo,info[0] + 256);
				}
				if (data == 256 + 0x0e && keyWin != 0) {
					putFIFOBuffer32(&keyWin->task->fifo,8 + 256); //直接发转换后的TAB键码+偏移，减少资源消耗
				}
				if(data == 0x1c + 256 && keyWin != 0){ //回车键
					putFIFOBuffer32(&keyWin->task->fifo,10 + 256); //发送回车键键盘码
				}
				if(data == 0x0f + 256){ //TAB
					//sheet_updown(keyWin,shtctl->top - 1);
					//sheet_updown(sht,shtctl->top - 1);
					keywinOff(keyWin);
					j = keyWin->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					keyWin = shtctl->sheets[j];
					keywinOn(keyWin);
					//sheet_updown(keyWin,shtctl->top - 1);
					////cursorC = keywinOn(keyWin,sht_win,cursorC);					
				}		
				if(data == 0x2a + 256){ //左Shift被按下
					keyShift |= 1;
				}
				if(data == 0x36 + 256){ //由Shift被按下
					keyShift |= 2;
				}
				if(data == 0xaa + 256){
					keyShift &= ~1;
				}
				if(data == 0xb6 + 256){
					keyShift &= ~2;
				}
				if(data == 0x3a + 256){ //CapsLock被按下
					keyLEDS ^= 4;
					putFIFOBuffer32(&FIFOKeyCMD,KEYCMD_LED_STATUS);
					putFIFOBuffer32(&FIFOKeyCMD,keyLEDS);
				}
				if(data == 0x45 + 256){ //NumLock被按下
					keyLEDS ^= 1;
					putFIFOBuffer32(&FIFOKeyCMD,KEYCMD_LED_STATUS);
					putFIFOBuffer32(&FIFOKeyCMD,keyLEDS);
				}
				if (data == 0x46 + 256){
					keyLEDS ^= 1;
					putFIFOBuffer32(&FIFOKeyCMD,KEYCMD_LED_STATUS);
					putFIFOBuffer32(&FIFOKeyCMD,keyLEDS);
				}	
				if (data == 0x3c + 256 && keyShift != 0){ //Shift+F2打开新的控制台窗口
					if(keyWin != 0){
						keywinOff(keyWin);
					}
					////keywinOff(keyWin);
					keyWin = openConsole(shtctl);
					//sheet_slide(keyWin,32,4);
					sheet_slide(keyWin,CX,CY); 
					sheet_updown(keyWin,shtctl->top);
					//CX += (bootInfo->screen_width + 8)%bootInfo->screen_width;CY += (bootInfo->screen_height + 28)%bootInfo->screen_height;
					CX += 8;CY += 28;
					if(CX + CONSOLE_WIDTH >= bootInfo->screen_width){
						CX = 8;
					}
					if(CY + CONSOLE_HEIGHT >= bootInfo->screen_height){
						CY = 8;
					}
					keywinOn(keyWin);
					
				}				
				if (data == 0x3e + 256 && keyShift != 0 && keyWin != 0){
					task = keyWin->task;
					if (task != 0 && task->tss.ss0 != 0){	//F4强制结束应用程序，应用运行是ss0必非0
						io_cli();	//关中断，禁止任务切换
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						sysprintl(task->console,"\n[F4]Forced kill the App.\n");
						io_sti(); //开中断
					}
				}
				if(data == 0x57 + 256 && shtctl->top > 2){ //F11被按下，切换窗口，底层移至顶端
					sheet_updown(shtctl->sheets[1],shtctl->top - 1);
				}
				if(data == 0xfa + 256){ //数据写入成功
					keyCMDWait = -1;
				}
				if(data == 0xfe + 256){ //数据写入失败
					waitKBCReady(); //等待键盘控制电路命令执行完毕进入稳定状态并返回信息
					io_out8(PORT_KEYDAT,keyCMDWait); //将缓冲区数据写入键盘数据端口	
				}	
			}
			else if(data >=512 && data <=767){
				if(decodeMouse(&mouseCursor,data - 512) != 0){
					vmx += mouseCursor.mx;vmy += mouseCursor.my;
					if(vmx < 0){
						vmx = 0;
					}
					if(vmy <0){
						vmy = 0;
					}
					if(vmx > bootInfo->screen_width - 1){
						vmx = bootInfo->screen_width - 1;
					}
					if(vmy > bootInfo->screen_height - 1){
						vmy = bootInfo->screen_height - 1;
					}
					sprintf(info, "(%3d, %3d)", vmx, vmy);
					windowDraw8(buf_back,bootInfo->screen_width,deviceInfoWindow,0,REFRESH_ALL);
					wordsDraw8(buf_back,bootInfo->screen_width,deviceInfoWindow.X + 6,deviceInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 5,deviceInfoWindow.foreColor,"Type:PS/2 Mouse");
					wordsDraw8(buf_back,bootInfo->screen_width,deviceInfoWindow.X + 6,deviceInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 25,deviceInfoWindow.foreColor,info);	
					sheet_refresh(sht_back, deviceInfoWindow.X, deviceInfoWindow.Y,deviceInfoWindow.X + deviceInfoWindow.width + WINDOW_LINE_WIDTH,deviceInfoWindow.Y + deviceInfoWindow.height + WINDOW_LINE_WIDTH);
					sheet_slide(sht_mouse, vmx, vmy); 					
					windowDraw8(buf_back,bootInfo->screen_width,memeryInfoWindow,0,REFRESH_ALL); //绘制内存信息窗口
					sprintf(info, "Memery:%dMB", (memeryList->total_size / (1024 * 1024)));  //输出内存总量信息
					wordsDraw8(buf_back,bootInfo->screen_width,memeryInfoWindow.X +  2,memeryInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 2,memeryInfoWindow.foreColor,info); //绘制内存总量信息
					sprintf(info,"Free:%dKB",(memeryList->total_free_size / 1024)); //输出空闲内存信息
					wordsDraw8(buf_back,bootInfo->screen_width,memeryInfoWindow.X +  2,memeryInfoWindow.Y + WINDOW_CAPTION_HEIGHT + 18,memeryInfoWindow.foreColor,info); //绘制内存余量信息
					sheet_refresh(sht_back, memeryInfoWindow.X, memeryInfoWindow.Y,memeryInfoWindow.X + memeryInfoWindow.width + WINDOW_LINE_WIDTH,memeryInfoWindow.Y + memeryInfoWindow.height + WINDOW_LINE_WIDTH); //刷新图层列表						
					
					if((mouseCursor.mbutton & 0x01) != 0){ //0x01为鼠标左键键码
						if(mmx < 0) {
							for (j = shtctl->top - 1;j > 0;j--){//自上而下扫描图层，判断鼠标所落图层区域
								sht = shtctl->sheets[j];
								x = vmx - sht->vx0;
								y = vmy - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {
									if (sht->buf[y * sht->bxsize + x] != sht->col_inv){
										sheet_updown(sht,shtctl->top - 1);
										if(sht != keyWin) {
											////cursorC = keywinOff(keyWin, sht_win, cursorC, cursorX);
											keywinOff(keyWin);
											keyWin = sht;
											////cursorC = keywinOn(keyWin, sht_win, cursorC);
											keywinOn(keyWin);
										}
										if(3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
											mmx = vmx;
											mmy = vmy;
											//keywinOn(keyWin,sht_win,cursorC);
										}
										if(sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19){
											if ((sht->flags & 0x10) != 0){ //应用程序窗口
												task = sht->task;
												io_cli(); //关中断，禁止任务切换
												task->tss.eax = (int) &(task->tss.esp0);
												task->tss.eip = (int) asm_end_app;
												sysprintl(task->console,"[ML]Mouse closed the app.\n");
												io_sti(); //开中断
											}
											else{ //控制台窗口
												task = sht->task;
												io_cli();
												putFIFOBuffer32(&task->fifo,4); //发送关闭信号
												io_sti();
											}											
										}										
										break;
									}
								}
							}
						} else {
							x = vmx - mmx;
							y = vmy - mmy;
							sheet_slide(sht, sht->vx0 + x, sht->vy0 + y);
							mmx = vmx;
							mmy = vmy;
						}
					} else {
						mmx = -1;
					}
					if((mouseCursor.mbutton & 0x02) != 0 && rightFlag == 1){
					if(keyWin != 0){
						keywinOff(keyWin);
					}
					////keywinOff(keyWin);
					keyWin = openConsole(shtctl);
					//sheet_slide(keyWin,32,4);
					sheet_slide(keyWin,CX,CY); 
					sheet_updown(keyWin,shtctl->top);
					//CX += (bootInfo->screen_width + 8)%bootInfo->screen_width;CY += (bootInfo->screen_height + 28)%bootInfo->screen_height;
					CX += 8;CY += 28;
					if(CX + CONSOLE_WIDTH >= bootInfo->screen_width){
						CX = 8;
					}
					if(CY + CONSOLE_HEIGHT >= bootInfo->screen_height){
						CY = 8;
					}
					keywinOn(keyWin);
						rightFlag = 0;
					}
					else{
						rightFlag = 1;
					}
				}
			}
			else if(data >= 768 && data <=1023){ //关闭命令行
				closeConsole(shtctl->sheets0 + (data - 768)); //根据句柄关闭图层，偏移为768
			}			
			else if(data == 10){
				labelDraw(sht_back,0,64,COL8_FFFFFE,DESK_BCOLOR,"10(sec)",10); //绘制10秒提示
				//sprintf(info,"%010d",myCount); //输出测试数据
				//labelDraw(sht_win,16,28,COL8_000000,COL8_FFFFFE,info,10); //绘制标签,测试用	
			}
			else if(data == 3){
				labelDraw(sht_back,0,80,COL8_FFFFFE,DESK_BCOLOR,"3(sec)",10); //绘制3秒提示
				//myCount = 0; //启动3秒后myCount置0，测试用				
			}						
			/* else if(data <= 1){
				if(data != 0){
					initTimer(timer,&FIFOBuffer32,0); //定时器超时信息置0
					if(cursorC >= 0){
						cursorC = COL8_000000;
					}
				}
				else{
					initTimer(timer,&FIFOBuffer32,1);
					if(cursorC >= 0){
						cursorC = COL8_FFFFFE;						
					}
				}
				setTimer(timer,50);
				if(cursorC >= 0){
					boxDraw8(buf_win,sht_win->bxsize,cursorX,28,cursorX + 7,43,cursorC); //绘制光标
				}
				sheet_refresh(sht_win,cursorX,28,cursorX + 8,44);					
			} */
		}
	}
}

void keywinOff(SHEET *keyWin){
	refreshWindowCaptionx(keyWin,1);
	if ((keyWin->flags & 0x20) != 0) {
		putFIFOBuffer32(&keyWin->task->fifo, 3);
	}
}

void keywinOn(SHEET *keyWin){
	refreshWindowCaptionx(keyWin,0);
	if ((keyWin->flags & 0x20) != 0) {
		putFIFOBuffer32(&keyWin->task->fifo, 2);
	}
}

void refreshWindowCaptionx(SHEET *cover,int focus){
	int x,y,xsize;
	char color,oldColor,newColor,boldcolor,bnewColor;
	xsize = cover->bxsize;
	if(focus == 0){
		newColor = COL8_0078D7;
		oldColor  = COL8_E1E1E1;
		bnewColor = COL8_E81123;
		boldcolor = COL8_E1E1E1; 
	}
	else{
		newColor = COL8_E1E1E1;
		oldColor = COL8_0078D7;
		bnewColor = COL8_E1E1E1;
		boldcolor = COL8_E81123;
	}
	for (y = 1;y <= 21;y++) {
		for (x = 1; x <= xsize - 4;x++){
			color = cover->buf[y * xsize + x];
			if (color == oldColor && x <= xsize - 22){
				color = newColor;
			}
			else if(color == boldcolor) {
				color = bnewColor;
			}
			cover->buf[y * xsize + x] = color;
		}
	}
	sheet_refresh(cover,1,1, xsize,22);	
}

////int consoleX = 0; int consoleY = 0;
//consoleX = WINDOW_X;consoleY = WINDOW_Y;
//consoleX = CONTROL_WINDOW.X;console
//CONTROL_WINDOW.X = CONTROL_WINDOW.X + 8;
//CONTROL_WINDOW.Y += 28;
//WINDOW consoleWindow = CONTROL_WINDOW;
////consoleX += 8;consoleX += 28;
////consoleX = consoleX + 8;
////BOX *consoleBox = ControlBox;
////consoleBox.bx0 = consoleBox.bx0 +  8;consoleBox.bx1 =  consoleBox.bx1 + 8;
SHEET *openConsole(SHTCTL *shtctl){
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	SHEET *sht = sheet_alloc(shtctl);
	if(sht == 0){
		sysprintl(taskNow()->console,"Create new console fail!");
		return 0;
	}
	unsigned char *buf = (unsigned char *)memeryAlloc4k(memeryList,CONSOLE_WIDTH * CONSOLE_HEIGHT);
	TASK *task = taskAlloc();
	int *cons_fifo = (int *)memeryAlloc4k(memeryList, 128 * 4);
	//sht->vx0 = CX;sht->vy0 = consoleY;
	sheet_setbuf(sht,buf,CONSOLE_WIDTH,CONSOLE_HEIGHT,-1); //设定控制台图层
	createWindow(buf,CONSOLE_WIDTH,CONSOLE_HEIGHT,"Console",CONTROL_WINDOW,0); //创建控制台窗口
	task->consoleStack = memeryAlloc4k(memeryList, 64 * 1024); //保存控制台地址
	task->tss.esp = task->consoleStack + 64 * 1024 - 12;	
	task->tss.esp = memeryAlloc4k(memeryList, 64 * 1024) + 64 * 1024 - 12;
	task->tss.eip = (int) &consoleTask;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memeryList->total_size; //地址为ESP+8
	//task->lang_mode = 0;
	taskRun(task, 2, 2);
	sht->task = task;
	sht->flags |= 0x20;
	initFIFOBuffer32(&task->fifo,cons_fifo,128,task);
	////sheet_slide(sht,consoleX,consoleY); //鼠标图层复位
	////consoleX = sht->vx0;consoleY = sht->vy0;
	return sht;
}

void closeConstask(TASK *task){ //关闭进程
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	taskSleep(task); //休眠，使关闭过程中不会切换到此任务，从休眠列表安全删除
	memeryFree4k(memeryList,task->consoleStack, 64 * 1024);
	memeryFree4k(memeryList,(int)task->fifo.buffer, 128 * 4);
	task->flag = 0; //替代taskFree(task);
}

void closeConsole(SHEET *sht){ //关闭窗口图层
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	TASK *task = sht->task;
	memeryFree4k(memeryList,(int)sht->buf, 256 * 165);
	sheet_free(sht);
	closeConstask(task);
}

void newConsole(SHTCTL *shtctl,SHEET *keyWin,int scrx,int scry){
	if(keyWin != 0){
		keywinOff(keyWin);
	}
	keyWin = openConsole(shtctl);
	sheet_slide(keyWin,CX,CY); 
	sheet_updown(keyWin,shtctl->top);
	CX += 8;CY += 28;
	if(CX + CONSOLE_WIDTH >= scrx){
		CX = 8;
	}
	if(CY + CONSOLE_HEIGHT >= scry){
		CY = 8;
	}
	keywinOn(keyWin);	
}

