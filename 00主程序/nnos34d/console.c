/**
 * ===================NNOS-graphics===========================
 *【控制台相关函数】【应用层】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

/*控制台主程序*/
void consoleTask(SHEET *sheet,unsigned int totalFreeSize){
	//TIMER *timer;
	TASK *task = taskNow(); //consoleTask需执行休眠，须知本身的Task所在内存地址，为避免传值，直接使用taskNow获取此地址
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	CONSOLE console;
	FILE_HANDLE fileHandle[8]; //文件处理器
	int data;
	int i;
	////int buffer[128];
	console.sheet = sheet;console.cursorX = 16,console.cursorY = 28;console.cursorC = -1;
	//*((int *)0x0fec) = (int)&console; //将控制台主函数地址保存在内存0x0fec供应用程序调用
	task->console = & console; //将任务控制台指针指向当前控制台
	char info[32],command[32] = {0};
	char version[20] = {0};
	int *fat = (int *)memeryAlloc4k(memeryList,4 * 2880); //为文件分配表申请空间，分配表有两张（一张为备份）
	
	unsigned char *fonts = (char *) * ((int *) 0x0fe8); //取得字库
	
	////initFIFOBuffer32(&task->fifo,buffer,128,task);
	task->console = &console;
	task->command = command;
	console.timer = timerAlloc();initTimer(console.timer,&task->fifo,1);setTimer(console.timer,50); //初始化定时器缓冲区
	readFAT(fat,(unsigned char *)(DISK_ADR + 0X000200)); //载入文件分配表
	
	for(i = 0;i < 8;i++){
		fileHandle[i].buffer = 0; //初始化文件处理器，状态标志位未使用
	}
	task->file_handle = fileHandle; //指向文件处理器
	task->fat = fat; //指向文件分配表
	
	if (fonts[4096] != 0xff) {	//判断是否已加载日文字库
	//if (fonts[5] != 0xff) {	//判断是否已加载日文字库
		//task->lang_mode = 1;
		task->lang_mode = 1;
	}
	else{
		task->lang_mode = 0;
	}
	task->lang_byte = 0; //半角模式
	task->lang_mode = 3;
	
	
	sprintf(version," NNRJ NNOS [version %s]",NNOS_VERSION);  //输出版本信息	
	labelDraw(console.sheet,8,console.cursorY,COL8_38CE2F,COL8_000000,version,32);
	newCMDLine(&console);
	labelDraw(console.sheet,8,console.cursorY,COL8_38CE2F,COL8_000000,"(c)2019 NNRJ Corporation. All Rights Reserved.",46);
	newCMDLine(&console);
	labelDraw(console.sheet,8,console.cursorY,COL8_38CE2F,COL8_000000,">",1);
	while(1){
		io_cli();
		if(getFIFOBufferStatus32(&task->fifo) == 0){
			taskSleep(task);
			io_sti();
		}
		else{
			data = getFIFOBuffer32(&task->fifo);
			io_sti();
			if(data <= 1){
				if(data != 0){
					initTimer(console.timer,&task->fifo,0);
					if(console.cursorC >= 0){
						console.cursorC = COL8_FFFFFE;
					}
				}
				else{
					initTimer(console.timer,&task->fifo,1);
					if(console.cursorC >= 0){
						console.cursorC = COL8_000000;
					}
				}
				setTimer(console.timer,50);
			}
			if(data == 2){ //光标显示
				console.cursorC = COL8_FFFFFF;
			}
			if(data == 3){ //光标隐藏
				console.cursorC = - 1;
				boxDraw8(console.sheet->buf,console.sheet->bxsize,console.cursorX,console.cursorY ,console.cursorX + 7,43,COL8_000000); //绘制光标
			}
			if (data == 4) { //控制台关闭
				exitCMD(&console,fat);
			}			
			if(256 <= data && data <=511){
				if(data == 8 + 256){ //删除键
					if(console.cursorX > 16){
						labelDraw(console.sheet,console.cursorX,console.cursorY,COL8_FFFFFE,COL8_000000," ",1); //输出字符
						console.cursorX -= 8;
					}
				}
				else if(data == 10 + 256){ //回车键
					labelDraw(console.sheet,console.cursorX,console.cursorY,COL8_FFFFFE,COL8_000000," ",1); //输出字符
					command[console.cursorX / 8 - 2] = 0; //定位用户输入	
					newCMDLine(&console);
					commandCMD(&console,fat,command);
					console.cursorX = 16;					
				}				
				else{
					if(console.cursorX < CONSOLE_WIDTH - 16){
						info[0] = data - 256;
						info[1] = 0;
						command[console.cursorX / 8 - 2] = data - 256;
						labelDraw(console.sheet,console.cursorX,console.cursorY,COL8_38CE2F,COL8_000000,info,1); //擦除
						console.cursorX += 8;
					}
				}
			}
			if(console.cursorC >= 0){
				boxDraw8(console.sheet->buf,console.sheet->bxsize,console.cursorX,console.cursorY,console.cursorX + 7,console.cursorY + 15,console.cursorC); //绘制光标
			}
			sheet_refresh(console.sheet,console.cursorX,console.cursorY,console.cursorX + 8,console.cursorY + 16);		
		}
	}
}

/*控制台命令*/
void commandCMD(CONSOLE *console,int *fat,char *command){
	if(strcmp(command,"mem" ) == 0 || strcmp(command,"free") == 0){ //mem,内存查询
		memCMD(console);			
	}
	else if(strcmp(command,"cls") == 0 || strcmp(command,"clear") == 0){ //cls,清屏
		clsCMD(console);
	}
	else if(strcmp(command,"version") == 0){ //version,版本信息
		versionCMD(console);
	}
	else if(strcmp(command,"sysinfo") == 0 || strcmp(command,"osinfo") == 0 || strcmp(command,"nnos") == 0){ //sysinfo,系统信息
		sysinfoCMD(console);						
	}
	else if(strcmp(command,"dir") == 0 || strcmp(command,"ls") == 0){ //dir,磁盘信息
		dirCMD(console);				
	}
	else if(strncmp(command,"type ",5) == 0 || strncmp(command,"cat ",4) == 0){
		typeCMD(console,fat,command);
	}
	else if(strncmp(command,"run ",4) == 0){
		runCMD(console,fat,command);
	}
	else if(strcmp(command,"exit") == 0){
		exitCMD(console,fat);
	}
	else if(strncmp(command,"start",6) == 0){
		startCMD(console,command);
	}
	else if(strncmp(command, "langmode ",9) == 0) {
		syslangCMD(console,command);
	}
	else if(strcmp(command,"shutdown") == 0){
		shutdownCMD(console,command);
	}
	else if(strncmp(command, "rm ",2) == 0 || strncmp(command, "del ",2) == 0){
		delCMD(console,fat,command);
	}
	/* else if(strncmp(command,"syslang ",8) == 0){
		syslangCMD(console,command);
	} */
	//else if(strncmp(command,"vim ",4){
		//runCMD(console,fat,command);
	//}
	/* else if(command[0] != 0){ //非法输入
		char myCmmand[16];
		//char myCmmand[32];
		sprintf(myCmmand, "run %s",command);
		if (runCMD(console,fat,myCmmand) == 0) {
			invalidCMD(console,command);
			//cons_putstr0(cons, "Bad command.\n\n");
		}	
	} */
	else if(command[0] != 0){ //非法输入
		//char myCmmand[16];
		//char myCmmand[32];
		//sprintf(myCmmand, "run %s",command);
		if (runCMD(console,fat,command) == 0) {
			invalidCMD(console,command);
			//cons_putstr0(cons, "Bad command.\n\n");
		}	
	}
	labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,">",1);
	//sysprintl(console,">");
}

/* void newCMDLine(CONSOLE *console){
	int x,y; //元素坐标
	if(console->cursorY <= CONSOLE_HEIGHT - 32){
		console->cursorY += 16;
	}
	else{
		for(y = 28;y< CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = console->sheet->buf[x + (y + 16) * console->sheet->bxsize];
			}
		}
		for(y = CONSOLE_HEIGHT - 28;y < CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console->sheet,8,28,CONSOLE_WIDTH - 16,CONSOLE_HEIGHT);
	}
	console->cursorX = 16;
} */

void newCMDLine(CONSOLE *console){
	int x,y; //元素坐标
	TASK *task = taskNow();
	if(console->cursorY <= CONSOLE_HEIGHT - 32){
		console->cursorY += 16;
	}
	else{
		for(y = 28;y< CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = console->sheet->buf[x + (y + 16) * console->sheet->bxsize];
			}
		}
		for(y = CONSOLE_HEIGHT - 28;y < CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console->sheet,8,28,CONSOLE_WIDTH - 16,CONSOLE_HEIGHT);
	}
	console->cursorX = 16;
	if(task->lang_mode == 1 && task->lang_byte != 0){
		console->cursorX +=16;
	}
}

/* void newCMDLine(CONSOLE *console){
	int x,y; //元素坐标
	if(console->cursorY <= CONSOLE_HEIGHT - 32){
		console->cursorY += 16;
	}
	else{
		for(y = 28;y< CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = console->sheet->buf[x + (y + 16) * console->sheet->bxsize];
			}
		}
		for(y = CONSOLE_HEIGHT - 28;y < CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console->sheet,8,28,CONSOLE_WIDTH - 16 - 2 ,CONSOLE_HEIGHT - 2);
	}
	console->cursorX = 16;
} */

/*控制台输出*/
void sysprint(CONSOLE *console,int charCode,char movFlag){
	char info[2];
	info[0] = charCode; //逐个打印字符
	info[1] = '\0';
	if(info[0] == 0x09){ //制表符
		while(1){ //寻找制表位，4的倍数
			labelDraw(console->sheet,console->cursorX,console->cursorY,COL8_38CE2F,COL8_000000," ",1); //填充空格
			console->cursorX += 8; //光标后移
			if(console->cursorX >= CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8){ //换行
				console->cursorX = 8;
				 newCMDLine(console);
			}
			if(((console->cursorX - 8) & 0x1f) == 0){ //可以被32整除(公因数为4、8、16、32，制表位)，32位恰可用二进制表示，故可直接使用&计算余数
				//size = (size + 0xfff) & 0xfffff000; //以4K为最小单位向上取整。相当于if((i & 0xfff) != 0){i = (i & 0xfffff000) + 0x1000;}
				break; //制表位找到，停止移动光标
			}
		}
	}
	else if(info[0] == 0x0a){ //换行符，换行回车一起处理，对0x0d回车符不再作处理
		console->cursorX = 8;
		newCMDLine(console);
	}
	else if(info[0] == 0x0d){ //回车符，换行回车一起处理，对0x0d回车符不再作处理
		//不作处理
	}	
	else{ //普通字符
		labelDraw(console->sheet,console->cursorX,console->cursorY,COL8_38CE2F,COL8_000000,info,1); //绘制信息
		//  newCMDLine(console); //换行
		console->cursorX += 8; //光标后移
		if(console->cursorX >= CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 8 ){ //最右端换行
			console->cursorX = 8;
			newCMDLine(console);
		}									
	}
	//return;
}

/*控制台清屏*/
void clsCMD(CONSOLE *console){
	COUNT x = 0;
	COUNT y = 0;
	for(y = 28;y < CONSOLE_HEIGHT -(CONSOLE_HEIGHT % 8) - 2;y++){
		for(x = 8;x < CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 2;x++){
			console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(console->sheet,7,28,CONSOLE_WIDTH -(CONSOLE_WIDTH % 8) - 1,CONSOLE_HEIGHT -(CONSOLE_HEIGHT % 8) - 1);
	console->cursorY = 28;	
}

/*文件列表*/
void dirCMD(CONSOLE *console){
	COUNT x = 0;
	char info[32];
	char fileName[9];
	for(x = 0;x < 9;x++){
		fileName[x] = 0;
	}
	FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	printLine(console);
	sysprintl(console,"[filename]  [exten]   [size]\n");
	printLine(console);	
	for(x = 0;x < FILE_INFO_MAX;x++){ //无文件信息（首字节为0x00，无文件信息）
		if(fileInfo[x].name[0] == 0x00){
			break;
		}
		if(fileInfo[x].name[0] != 0xe5){ //文件未删除（首字节为0xe5，文件已删除）
			if((fileInfo[x].type & 0x18) == 0){ //文件属性值正常，文件信息，非目录
				strncpy(fileName,fileInfo[x].name,8);
				sprintf(info,"%s    .%s   %7d\n",fileName,fileInfo[x].exten,fileInfo[x].size);
				sysprintl(console,info);															
			}								
		}
	}
	printLine(console);	
}

/*文件内容查看*/
void typeCMD(CONSOLE *console,int *fat,char *command){
	int myLen = 5;
	COUNT x = 0;
	COUNT y = 0;
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	//FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	FILE_INFO *fileInfo = 0;
	char *file;
	if(strncmp(command,"cat ",4) == 0){
		myLen = 4;
	}
	x = 0;y = 0; //初始化游标
	int flag = 0;
	char fileName[FILE_FULL_NAME_SIZE] = {0};
	char fileExten[FILE_EXTEN_SIZE] = {0};
	for(x = myLen;x < myLen + FILE_NAME_SIZE + 1;x++){	
		if(command[x] == '.'){
			strncpy(fileName,command + myLen,x - myLen); //解析文件名	
			flag = 1;
			break;
		}
	}
	if(flag == 0){
		strncpy(fileName,command + myLen,FILE_NAME_SIZE + myLen); //解析文件名
	}
	//convertToUppercase(fileName); //转为大写
	sprintf(fileName, "%-8s",fileName);	
	if(flag == 1){
		strncpy(fileExten,command + x + 1,3); //解析拓展名
		sprintf(fileExten, "%-3s",fileExten);		
	}
	else{
		sprintf(fileExten,"    \0");	
	}
	strcat(fileName,fileExten);	
	fileInfo = searchFile(fileName);
	if(fileInfo != 0){
		y = fileInfo->size; //文件长度
		//file = (char *) (fileInfo->cluster * 512 + 0x003e00 + DISK_ADR); //文件首地址
		file = (char *)memeryAlloc4k(memeryList,fileInfo->size); //为文件申请空间
		loadFile(fileInfo->cluster,fileInfo->size,file,fat,(char *)(0x003e00 + DISK_ADR)); //装载文件
		console->cursorX = 8; //复位光标
		for(x = 0;x < y;x++){ //遍历文件内容
			sysprint(console,file[x],1);
		}
		newCMDLine(console);
		memeryFree4k(memeryList,(int)file,fileInfo->size);
	}
	else{
		sysprintl(console,"File not fount.\n");						
	}	
}

/*文件删除*/
void delCMD(CONSOLE *console,int *fat,char *command){
	int myLen = 3;
	if(strncmp(command,"del ",4) == 0){
		myLen = 4;
	}
	else{
		myLen = 3;
	}
	COUNT x = 0;
	COUNT y = 0;
	////MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	//FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	////FILE_INFO *fileInfo = 0;
	//char *file;
	if(strncmp(command,"cat ",4) == 0){
		myLen = 4;
	}
	x = 0;y = 0; //初始化游标
	int flag = 0;
	char fileName[FILE_FULL_NAME_SIZE] = {0};
	char fileExten[FILE_EXTEN_SIZE] = {0};
	for(x = myLen;x < myLen + FILE_NAME_SIZE + 1;x++){	
		if(command[x] == '.'){
			strncpy(fileName,command + myLen,x - myLen); //解析文件名	
			flag = 1;
			break;
		}
	}
	if(flag == 0){
		strncpy(fileName,command + myLen,FILE_NAME_SIZE + myLen); //解析文件名
	}
	//convertToUppercase(fileName); //转为大写
	sprintf(fileName, "%-8s",fileName);	
	if(flag == 1){
		strncpy(fileExten,command + x + 1,3); //解析拓展名
		sprintf(fileExten, "%-3s",fileExten);		
	}
	else{
		sprintf(fileExten,"    \0");	
	}
	strcat(fileName,fileExten);	
	////fileInfo = searchFile(fileName);
	if(deleteFile(fileName) != 0){
		sysprintl(console,"Delete file successful.\n");
	}
	else{
		sysprintl(console,"Delete file fail!\n");
	}
	////if(fileInfo != 0){
		/* y = fileInfo->size; //文件长度
		//file = (char *) (fileInfo->cluster * 512 + 0x003e00 + DISK_ADR); //文件首地址
		file = (char *)memeryAlloc4k(memeryList,fileInfo->size); //为文件申请空间
		loadFile(fileInfo->cluster,fileInfo->size,file,fat,(char *)(0x003e00 + DISK_ADR)); //装载文件
		console->cursorX = 8; //复位光标
		for(x = 0;x < y;x++){ //遍历文件内容
			sysprint(console,file[x],1);
		} */
		////newCMDLine(console);
		////memeryFree4k(memeryList,(int)file,fileInfo->size);
	////}
	////else{
		////sysprintl(console,"File not fount.\n");						
	////}	
}

/*打印一行字符串*/
void sysprintl(CONSOLE *console,char *str){ //打印一行，遇'\0'即停止
	//while(*str != 0){
	while(*str != 0){
		sysprint(console,*str,1);
		str++;
	}
}

/*打印指定长度字符串*/
void sysprintx(CONSOLE *console,char *str,int len){ //打印指定长度字符串
	COUNT i = 0;
	for(i = 0;i < len;i++){
		sysprint(console,str[i],1);
	}
}

/*系统信息*/
void sysinfoCMD(CONSOLE *console){
	char info[32];
	printLine(console);		
	sprintf(info, "File_System:%s\n",NNOS_FILE_SYSTEM);  //输出文件系统信息
	sysprintl(console,info);
	sprintf(info, "Version:%s\n",NNOS_VERSION);  //输出系统版本信息
	sysprintl(console,info);
	sprintf(info, "Based:%s\n",NNOS_CPU_BASED);  //输出CPU架构信息
	sysprintl(console,info);
	sprintf(info, "Campany:%s\n",NNOS_COMPANY);  //输出公司信息
	sysprintl(console,info);
	sprintf(info, "Author:%s\n",NNOS_AUTHOR);  //输出作者信息
	sysprintl(console,info);
	sprintf(info, "Email:%s\n", NNOS_EMAIL);  //输出开发者邮箱信息
	sysprintl(console,info);
	sprintf(info, "Update_Date:%s\n",NNOS_UPDATE_DATE);  //输出升级日期信息
	sysprintl(console,info);
	printLine(console);		
}


/*版本信息*/
void versionCMD(CONSOLE *console){
	sysprintl(console,strcat("version:",NNOS_VERSION));
	newCMDLine(console);
}

/*内存信息*/
void memCMD(CONSOLE *console){
	char info[32];
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表	
	printLine(console);		
	sprintf(info, "Memery:%dMB\n", (memeryList->total_size / (1024 * 1024)));  //输出内存总量信息
	sysprintl(console,info);
	sprintf(info,"Free:%dKB\n",(memeryList->total_free_size / 1024)); //输出空闲内存信息
	sysprintl(console,info);
	printLine(console);	
}

/*运行可执行文件*/
int runCMD(CONSOLE *console,int *fat,char *command){
	int myLen = 0;
	if(strncmp(command,"run ",4) == 0){
		myLen = 4;
	}
	else{
		myLen = 0;
	}
	int x,y;
	COUNT i = 0;
	int gdtSize,dataSize,esp,dataNex;
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	//SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)GDT_ADR; //定义GDT并指向系统GDT
	SHTCTL *coverList;
	SHEET *cover;
	//FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	FILE_INFO *fileInfo = 0;
	char *file = 0;char *data = 0;
	TASK *task = taskNow();
	x = 0;y = 0; //初始化游标
	int flag = 0;
	char fileName[FILE_FULL_NAME_SIZE] = {0};
	char fileExten[FILE_EXTEN_SIZE] = {0};
	for(x = myLen;x < myLen + FILE_NAME_SIZE + 1;x++){
		//flag = 2;
		//fileName[x] = command[x];
		if(command[x] == '.' || command[x] == ' '){
			strncpy(fileName,command + myLen,x - myLen); //解析文件名
			//sysprintl(console,fileName);
			flag = 1;
			break;
		}
	}
	//sysprintl(console,flag);
	if(flag == 0){
		strncpy(fileName,command + myLen,FILE_NAME_SIZE + myLen); //解析文件名
		// for(x = myLen;x < myLen + FILE_FULL_NAME_SIZE + 1;x++){
		//	if(command[x] == ' '){
		//		strncpy(fileName,command + myLen - 1,x - myLen - 1); //解析文件名
		//		flag = 1;
		//	}
		//} 
		//flag = 1;
	}
	/* if(flag == 0){
		//sysprintl(console,flag);
		return 0;
	} */
	//convertToUppercase(fileName); //转为大写
	sprintf(fileName,"%-8sNEX",fileName);
	//flag = 0;
	/* strncpy(fileExten,command + x + 1,3); //解析拓展名
	//convertToUppercase(fileExten); //转为大写
	sprintf(fileExten, "%-3s",fileExten);
	if(strcmp(fileExten,"   ") == 0){
		strcpy(fileExten,"NEX");
	} */

	strcat(fileName,fileExten);							
	//fileInfo = searchFile(fileName);
	fileInfo = searchFile(fileName);
	strncpy(fileName,fileInfo->name+8,3);
	//if(fileInfo != 0 && (strcmp(fileName,"NEX") == 0 || strcmp(fileName,"   ") == 0)){
	//if(fileInfo != 0 && strcmp(fileInfo->exten,"NEX") == 0){
	//if(fileInfo != 0 && flag != 0){
	if(fileInfo != 0){
		file = (char *)memeryAlloc4k(memeryList,fileInfo->size); //为文件申请空间
		//data = (char *)memeryAlloc4k(memeryList,64 * 1024); //为数据申请空间
		*((int *)0xfe8) = (int)file; //将应用程序传来的内容段地址存放在0xfe8
		loadFile(fileInfo->cluster,fileInfo->size,file,fat,(char *)(0x003e00 + DISK_ADR)); //装载文件
		if(fileInfo->size >= 36 && strncmp(file + 4,"NNOS",4) == 0 && *file == 0x00){ //文件大小大于36，偏移4开始向后4个字节为“NNOS”,开头为空，为可执行文件
			gdtSize = *((int *)(file + 0x0000));
			esp = *((int *)(file + 0x000c));
			dataSize = *((int *)(file + 0x0010));
			dataNex = *((int *)(file + 0x0014));
			data = (char *)memeryAlloc4k(memeryList,gdtSize); //为文件数据申请空间
			//*((int *)0xfe8) = (int)data;
			task->base = (int)data;
			setSegmDesc(task->ldt + 0,fileInfo->size - 1,(int)file,CODE32_ER_PRE + 0x60);
			setSegmDesc(task->ldt + 1,gdtSize - 1,(int)data,DATE32_RW_PRE + 0x60);
			//setSegmDesc(gdt + 1003,fileInfo->size - 1,(int)file,CODE32_ER_PRE + 0x60); //设定应用程序段，3号，1~2号由task.c使用
			////setSegmDesc(gdt + task->selector / 8 + 1000,fileInfo->size - 1,(int)file,CODE32_ER_PRE + 0x60); //设定应用程序段，3号，1~2号由task.c使用，段号除以8落在3~1002，加1000得1003~2002，避免覆盖
			//setSegmDesc(gdt + 1004,gdtSize - 1,(int)data,DATE32_RW_PRE + 0x60); //设定应用数据段，访问权限加0x60置为应用程序
			////setSegmDesc(gdt + task->selector / 8 + 2000,gdtSize - 1,(int)data,DATE32_RW_PRE + 0x60); //设定应用数据段，访问权限加0x60置为应用程序，落在2003~3002
			for(i = 0;i < dataSize;i++){
				data[esp + i] = file[dataNex + i];
			}
			//run_app(0x1b,1003*8,esp,1004*8,&(task->tss.esp0)); //运行时传参，在TSS中注册OS用段地址和ESP。0x0018中存放E9,即JMP指令机器码，跳入程序入口
			//run_app(0x1b,task->selector + 1000 * 8,esp,task->selector + 2000 * 8,&(task->tss.esp0)); //运行时传参，在TSS中注册OS用段地址和ESP。0x0018中存放E9,即JMP指令机器码，跳入程序入口
			run_app(0x1b, 0 * 8 + 4, esp, 1 * 8 + 4, &(task->tss.esp0)); //段号乘以8+4，代表非GDT段，使用LDT，每个人物都有专用LDT
			coverList = (SHTCTL *) *((int *) 0x0fe4);
			for(i = 0;i < MAX_SHEETS;i++){
				cover = &(coverList->sheets0[i]);
				if((cover->flags & 0x11) == 0x11 && cover->task == task){ //检查图层残留
				//if (cover->flags != 0 && cover->task == task){ //检查图层残留
					sheet_free(cover); //释放残留图层
				}
			}
			for(i = 0;i < 8;i++){ //关闭文件
				if (task->file_handle[i].buffer != 0) {
					memeryFree4k(memeryList,(int) task->file_handle[i].buffer,task->file_handle[i].size);
					task->file_handle[i].buffer = 0;
				}
			}			
			timerCancelAll(&task->fifo);
			memeryFree4k(memeryList,(int)data,gdtSize); 
			task->lang_byte = 0; //恢复半角模式
			newCMDLine(console);
			return 1;
		}
		else{
			sysprintl(console,"APP file format error.");
			////return 0;
		}
		memeryFree4k(memeryList,(int)file,fileInfo->size); //执行完毕释放应用程序所占内存
		newCMDLine(console);
	}
	else{	
		sysprintl(console,"File not fount.\n");
		////return 0;
	}
	return 0;
}

/*退出控制台命令*/
void exitCMD(CONSOLE *console,int *fat){
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	TASK *task = taskNow();
	SHTCTL *shtctl = (SHTCTL *) *((int *) 0x0fe4);
	FIFO_BUFFER32 *fifo = (FIFO_BUFFER32 *) *((int *) 0x0fec);
	timerCancel(console->timer); //关闭窗口光标占用的定时器
	memeryFree4k(memeryList,(int)fat,4 * 2880); //释放fat所占内存空间
	io_cli();
	putFIFOBuffer32(fifo,console->sheet - shtctl->sheets0 + 768);	//768～1023
	io_sti();
	while(1){
		taskSleep(task); //休眠，后续由taskA将其关闭
	}
}

/*在新的窗口运行程序*/
void startCMD(CONSOLE *console, char *command){
	//char myCmmand[16];
	SHTCTL *shtctl = (SHTCTL *) *((int *) 0x0fe4);
	SHEET *sht = openConsole(shtctl);
	////FIFO_BUFFER32 *fifo = &sht->task->fifo;
	////int i;
	sheet_slide(sht,32,4);
	sheet_updown(sht,shtctl->top);
	//strcpy(myCmmand,command + 6); //解析拓展名
	//for (i = 0;myCmmand[i] != 0;i++) {
	/* for (i = 6;command[i] != 0;i++) {
		putFIFOBuffer32(fifo,command[i] + 256); //将命令字符串逐字写入新窗口FIFO
	} */
	/* if(strcmp(command,"start") != 0){
		putFIFOBuffer32(fifo,10 + 256); //模拟Enter键
		startCMD(console,command);
		command = 0;
	} */
	newCMDLine(console);
}

/* void cmd_start(struct CONSOLE *cons, char *cmdline, int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl, memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht, 32, 4);
	sheet_updown(sht, shtctl->top);
	for (i = 6; cmdline[i] != 0; i++) {
		fifo32_put(fifo, cmdline[i] + 256);
	}
	fifo32_put(fifo, 10 + 256);	
	cons_newline(cons);
	return;
} */

/* void syslangCMD(CONSOLE *console,char *command){
	TASK *task = taskNow();
	unsigned char mode = command[8] - '0';
	// switch(mode){
	//	case 1:{
	//		task->lang_mode = mode;
	//		break;
	//	}
	//	default:{
	//		sysprintl(console,"Mode number error.\n");
	//	}
	//}
	if(mode <= 1){
		task->lang_mode = mode;
	}
	else{
		sysprintl(console,"Mode number error.\n");
	}
	newCMDLine(console);
} */

/* void syslangCMD(CONSOLE *console, char *command){
	TASK *task = taskNow();
	unsigned char mode = command[9] - '0';
	if (mode <= 3) {
		task->lang_mode = mode;
	}
	else{
		sysprintl(console, "Mode number error.\n");
	}
	newCMDLine(console);
} */

void syslangCMD(CONSOLE *console, char *command){
//void cmd_langmode(struct CONSOLE *cons, char *cmdline){
	struct TASK *task = taskNow();
	unsigned char mode = command[9] - '0';
	if (mode <= 3) {
		task->lang_mode = mode;
	} else {
		sysprintl(console, "Mode number error.\n");
	}
	newCMDLine(console);
}

void shutdownCMD(CONSOLE *console, char *command){
	//io_cli();
	//io_hlt(); 
	asm_shutdown(); 
	sysprintl(console,"Shutdown fail(0x00000001).");
	newCMDLine(console);
}

/*无效的命令*/
void invalidCMD(CONSOLE *console,char *command){
	char info[32];
	if(strlen(command)  < 12){
		sprintf(info,"\'%s\' is invalid command.\n",command); //输出信息
	}
	else{
		char commandSub[6],commandSub2[4];
		strncpy(commandSub,command,5);
		strncpy(commandSub2,command+(strlen(command) - 3),3);
		commandSub[5] = '\0';
		commandSub2[3] = '\0';
		sprintf(info,"\'%s...%s\' is invalid command.\n",commandSub,commandSub2); //输出信息
	}
	sysprintl(console,info);
}

/*打印固定长度横线*/
void printLine(CONSOLE *console){	
	sysprintl(console,"-----------------------------\n");
}

/*打印指定长度横线*/
void printLinex(CONSOLE *console,int len){
	int i;
	for(i = 0;i < len;i++){
		sysprintx(console,"-",1);
	}
	newCMDLine(console);	
}

/*打印地址*/
void printAddress(CONSOLE *console,unsigned char addr){
	char temp[32];
	sprintf(temp,"%d ",(int)addr);
	sysprintl(console,temp);
}

int *sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax){
	//int i;
	TASK *task = taskNow(); //取得当前任务
	//int fileBase = *((int *)0xfe8); //取得可执行文件文件内容段地址,CS
	int fileBase = task->base; //取得可执行文件数据段地址,CS
	//CONSOLE *console = (CONSOLE *) *((int *) 0x0fec); //控制台地址
	CONSOLE *console = task->console; //控制台地址
	//MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	//TASK *task = taskNow();
	SHTCTL *coverList = (struct SHTCTL *) *((int *) 0x0fe4); //图层列表地址
	SHEET *cover; //定义图层
	FILE_INFO *fileInfo;
	FILE_HANDLE *fileHandle;
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	int *reg = &eax + 1; //修改PUSHAD保存在寄存器中的值。eax存放窗口句柄，用于刷新窗口
	int data;
	switch(edx){
		case 1:{ //接口1，单个字符打印
			sysprint(console,eax & 0xff,1);break;
		}
		case 2:{ //接口2，打印一行字符串
			sysprintl(console,(char *)ebx + fileBase);break;
		}
		case 3:{ //接口3，打印指定长度字符串
			sysprintx(console,(char *)ebx + fileBase,ecx);break;
		}
		case 4:{ //接口4，返回操作系统
			return &(task->tss.esp0);
		}
		//case 123456789:{ //测试用API漏洞
			//((char *)0x00102600) = 0;
		//}
		case 5:{ //接口5，创建窗口
			cover = sheet_alloc(coverList); //ebx为窗口缓冲区，esi为宽度，edi为高度，eax为透明色,ecx为窗口名称,edx=5
			cover->task = task;
			cover->flags |= 0x10;
			sheet_setbuf(cover,(char *)ebx + fileBase,esi,edi,eax);
			createWindow((char *)ebx + fileBase,esi,edi,(char *)ecx + fileBase,INFO_WINDOW,0); //创建测试窗口
			//make_window8((char *)ebx + fileBase,esi,edi,(char *)ecx + fileBase,0);
			//sheet_slide(cover,450,120);
			//sheet_updown(cover,3); //图层在控制台窗口之上
			////sheet_slide(cover,(coverList->xsize - esi) / 2,(coverList->ysize - edi) / 2);
			sheet_slide(cover,((coverList->xsize - esi) / 2) & ~3,(coverList->ysize - edi) / 2); //窗口横坐标为4的倍数，AND运算取整,出现在屏幕中央
			sheet_updown(cover, coverList->top); //~3意为将3的每个比特取反，即0xfffffffc
			reg[7] = (int)cover;
			break;
		}
		case 6:{ //接口6，在窗口输出字符
			cover=(SHEET *)(ebx & 0xfffffffe); //与0xfffffffe作与运算，按2的倍数取整（有效图层地址皆为偶数）
			wordsDraw8(cover->buf,cover->bxsize,esi,edi,eax,(char *)ebp + fileBase);
			if((ebx & 1) == 0){ //校验刷新标志位
				sheet_refresh(cover,esi,edi,esi+ecx * 8,edi + 16);
			}			
			break;
		}
		case 7:{ //接口7，在窗口绘制色块
			cover=(SHEET *)(ebx & 0xfffffffe);
			boxDraw8(cover->buf,cover->bxsize,eax,ecx,esi,edi,ebp); //绘制色块
			if((ebx & 1) == 0){ //校验刷新标志位
				sheet_refresh(cover,eax,ecx,esi + 1,edi + 1);
			}
			break;
		}
		case 8:{ //接口8，初始化应用程序内存
			initMemeryManager((MEMERY_LIST *)(ebx + fileBase));
			ecx &= 0xfffffff0; //单位:16字节
			memeryFree((MEMERY_LIST *)(ebx + fileBase),eax,ecx); //eax,地址;ecx，大小
			break;
		}
		case 9:{ //接口9，为应用程序分配内存
			ecx = (ecx + 0x0f) & 0xfffffff0; //单位:16字节
			reg[7] = memeryAlloc((MEMERY_LIST *)(ebx + fileBase),ecx); //ecx,大小;成功与否反馈给reg[7]
			break;
		}
		case 10:{ //接口10，释放应用程序内存
			ecx = (ecx + 0x0f) & 0xfffffff0; //单位:16字节
			memeryFree((MEMERY_LIST *)(ebx + fileBase),eax,ecx); //ecx,大小
			break;
		}
		case 11:{ //接口11，在窗口绘制彩点
			cover = (SHEET *)(ebx & 0xfffffffe);
			cover->buf[cover->bxsize * edi + esi] = eax;
			if((ebx & 1) == 0){ //校验刷新标志位
				sheet_refresh(cover,esi,edi,esi + 1,edi + 1);
			}
			break;
		}
		case 12:{ //接口12，刷新窗口
			cover = (SHEET *)ebx;
			sheet_refresh(cover,eax,ecx,esi,edi);			
			break;
		}
		/* case 13:{ //接口13，在窗口绘制彩线
			cover = (SHEET *)(ebx & 0xfffffffe);
			syslinewin(cover,eax,ecx,esi,edi,ebp);
			if((ebx & 1) == 0){ //校验刷新标志位
				sheet_refresh(cover,eax,ecx,esi + 1,edi + 1);
			}
			break;
		} */
		case 13:{ //接口13，在窗口绘制彩线
			cover = (SHEET *) (ebx & 0xfffffffe);
			syslinewin(cover,eax,ecx,esi,edi,ebp);
			if((ebx & 1) == 0){ //校验刷新标志位
				if (eax > esi){ //将左上角、右下角坐标比较，防止刷新错位
					data = eax;
					eax = esi;
					esi = data;
				}
				if (ecx > edi){
					data = ecx;
					ecx = edi;
					edi = data;
				}
				sheet_refresh(cover,eax,ecx,esi + 1,edi + 1);
			}			
			break;
		}
		case 14:{ //接口14，释放（关闭）应用程序窗口图层
			sheet_free((struct SHEET *) ebx);
			break;
		}	
		case 15:{ //接口15，获取用户键盘输入
			while(1){ //等待键盘输入
				io_cli(); //关中断
				if (getFIFOBufferStatus32(&task->fifo) == 0){ //查看缓冲区状态
					if (eax != 0){ //判断输入标志位
						taskSleep(task); //空闲让进
					}
					else{
						io_sti(); //开中断
						reg[7] = -1;
						return 0;
					}
				}
				data = getFIFOBuffer32(&task->fifo); //读取缓冲区数据
				io_sti(); //开中断
				if(data <= 1){ //数据为0或1，设定光标定时器
					initTimer(console->timer, &task->fifo, 1);
					setTimer(console->timer, 50);
				}
				if(data == 2){ //数据为2，更改光标颜色
					console->cursorC = COL8_FFFFFE; //墨白色
				}
				if(data == 3){
					console->cursorC = -1; //透明色
				}
				if (data >= 256) {
					reg[7] = data - 256;
					return 0;
				}			
			}
			break;
		}
		case 16:{ //接口16，为应用程序申请定时器
			reg[7] = (int)timerAlloc();
			((TIMER *) reg[7])->flag2 = 1; //允许定时器自动关闭
			break;
		}
		case 17:{ //接口17，初始化应用程序定时器
			initTimer((TIMER *) ebx,&task->fifo,eax + 256); //偏移256
			break;
		}
		case 18:{ //接口18，设定应用程序定时器
			setTimer((TIMER *)ebx,eax);
			break;
		}
		case 19:{ //接口19，释放化应用程序定时器
			freeTimer((TIMER *)ebx);
			break;
		}
		case 20:{ //接口20，蜂鸣发声器
			if (eax == 0){
				data = io_in8(0x61);
				io_out8(0x61, data & 0x0d);
			}
			else{
				data = 1193180000 / eax;
				io_out8(0x43, 0xb6);
				io_out8(0x42, data & 0xff);
				io_out8(0x42, data >> 8);
				data = io_in8(0x61);
				io_out8(0x61, (data | 0x03) & 0x0f);
			}
			break;
		}
		case 21:{ //接口21，文件处理器:打开文件
			for(data = 0;data < 8;data++) {
				if (task->file_handle[data].buffer == 0){
					break;
				}
			}
			fileHandle = &task->file_handle[data];
			reg[7] = 0;
			if(data < 8) {
				fileInfo = file_search2((char *)ebx + fileBase,(FILE_INFO *)(DISK_ADR + 0x002600),224);
				//fileInfo = file_search((char *)ebx + fileBase,224);
				////fileInfo = file_search((char *)ebx + fileBase);
				//fileInfo = searchFile((char *)ebx + fileBase);
				if (fileInfo != 0){
					reg[7] = (int)fileHandle;
					fileHandle->buffer = (char *)memeryAlloc4k(memeryList,fileInfo->size);
					fileHandle->size = fileInfo->size;
					fileHandle->pos = 0;
					loadFile(fileInfo->cluster,fileInfo->size,fileHandle->buffer,task->fat,(char *)(DISK_ADR + 0x003e00));
				}
			}
			break;
		}
		case 22:{ //接口22，文件处理器:关闭文件
			fileHandle = (FILE_HANDLE *)eax;
			memeryFree4k(memeryList,(int)fileHandle->buffer,fileHandle->size);
			fileHandle->buffer = 0;			
			break;
		}
		case 23:{ //接口23，文件处理器:定位文件
			fileHandle = (FILE_HANDLE *)eax;
			if (ecx == 0){
				fileHandle->pos = ebx;
			}
			else if (ecx == 1){
				fileHandle->pos += ebx;
			}
			else if (ecx == 2){
				fileHandle->pos = fileHandle->size + ebx;
			}
			if(fileHandle->pos < 0){
				fileHandle->pos = 0;
			}
			if(fileHandle->pos > fileHandle->size){
				fileHandle->pos = fileHandle->size;
			}			
			break;
		}
		case 24:{ //接口24，文件处理器:获取文件大小
			fileHandle = (FILE_HANDLE *) eax;
			if(ecx == 0){
				reg[7] = fileHandle->size;
			}
			else if (ecx == 1){
				reg[7] = fileHandle->pos;
			}
			else if (ecx == 2){
				reg[7] = fileHandle->pos - fileHandle->size;
			}			
			break;
		}
		case 25:{ //接口25，文件处理器：读取文件
			fileHandle = (FILE_HANDLE *)eax;
			for(data = 0; data < ecx; data++){
				if (fileHandle->pos == fileHandle->size){
					break;
				}
				*((char *) ebx + fileBase + data) = fileHandle->buffer[fileHandle->pos];
				fileHandle->pos++;
			}
			reg[7] = data;			
			break;
		}
		case 26:{ //接口26，应用程序获取命令行输入
			data = 0;
			while(1){
				*((char *)ebx + fileBase + data) = task->command[data];
				if (task->command[data] == 0){
					break;
				}
				if (data >= ecx){
					break;
				}
				data++;
			}
			reg[7] = data;			
			break;
		}
		case 27:{ //接口27，获取语言模式
			reg[7] = task->lang_mode; //获取语言模式
			break;
		}
		/* default:{
			break;
		} */
	}
	return 0;
}

//=====已废弃的函数=========================================================

/* int *sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax){
	int fileBase = *((int *)0xfe8); //取得可执行文件文件内容段地址,CS
	CONSOLE *console = (CONSOLE *) *((int *) 0x0fec); //控制台地址
	//MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	TASK *task = taskNow();
	struct SHTCTL *coverList = (struct SHTCTL *) *((int *) 0x0fe4); //图层列表地址
	struct SHEET *cover; //定义图层
	int *reg = &eax + 1; //修改PUSHAD保存在寄存器中的值。eax存放窗口句柄，用于刷新窗口
	if(edx == 1){
		sysprint(console,eax & 0xff,1);
	}
	else if(edx == 2){
		sysprintl(console,(char *)ebx + fileBase);
	}
	else if(edx == 3){
		sysprintx(console,(char *)ebx + fileBase,ecx);
	}
	else if(edx == 4){
		return &(task->tss.esp0);
	}
	else if(edx == 5){
		cover = sheet_alloc(coverList); //ebx为窗口缓冲区，esi为宽度，edi为高度，eax为透明色,ecx为窗口名称,edx=5
		sheet_setbuf(cover,(char *)ebx + fileBase,esi,edi,eax);
		createWindow((char *)ebx + fileBase,esi,edi,(char *)ecx + fileBase,INFO_WINDOW,0); //创建测试窗口
		sheet_slide(cover,450,120);
		sheet_updown(cover,3); //图层在控制台窗口之上
		reg[7] = (int)cover;		
	}
	else if(edx == 6){
		cover=(SHEET *)ebx;
		wordsDraw8(cover->buf,cover->bxsize,esi,edi,eax,(char *)ebp + fileBase);
		sheet_refresh(cover,esi,edi,esi+ecx * 8,edi + 16);				
	}
	else if(edx == 7){
		cover=(SHEET *)ebx;
		boxDraw8(cover->buf,cover->bxsize,eax,ecx,esi,edi,ebp); //绘制光标
		sheet_refresh(cover,eax,ecx,esi + 1,edi + 1);		
	}
	else if(edx == 8){
		initMemeryManager((MEMERY_LIST *)(ebx + fileBase));
		ecx &= 0xfffffff0; //单位:16字节
		memeryFree((MEMERY_LIST *)(ebx + fileBase),eax,ecx); //eax,地址;ecx，大小		
	}
	else if(edx == 9){
		ecx = (ecx + 0x0f) & 0xfffffff0; //单位:16字节
		reg[7] = memeryAlloc((MEMERY_LIST *)(ebx + fileBase),ecx); //ecx,大小;成功与否反馈给reg[7]
	}
	else if(edx == 10){
		ecx = (ecx + 0x0f) & 0xfffffff0; //单位:16字节
		memeryFree((MEMERY_LIST *)(ebx + fileBase),eax,ecx); //ecx,大小		
	}
	else if(edx == 11){
		cover = (SHEET *) ebx;
		cover->buf[cover->bxsize * edi + esi] = eax;
		sheet_refresh(cover, esi, edi, esi + 1, edi + 1);		
	}
	return 0;
} */

/* int *sys_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	int fileBase = *((int *) 0xfe8);
	TASK *task = taskNow();
	CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
	SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	SHEET *sht;
	int *reg = &eax + 1;

	if (edx == 1) {
		sysprint(cons,eax & 0xff,1);
	} else if (edx == 2) {
		sysprintl(cons, (char *) ebx + fileBase);
	} else if (edx == 3) {
		sysprintx(cons, (char *) ebx + fileBase, ecx);
	} else if (edx == 4) {
		return &(task->tss.esp0);
	} else if (edx == 5) {
		sht = sheet_alloc(shtctl);
		sheet_setbuf(sht, (char *) ebx + fileBase, esi, edi, eax);
		//createWindow((char *) ebx + fileBase, esi, edi, (char *) ecx + fileBase, 0);
		createWindow((char *)ebx +  fileBase,esi,edi,(char *)ecx +  fileBase,INFO_WINDOW,0); //创建测试窗口
		sheet_slide(sht, 100, 50);
		sheet_updown(sht, 3);
		reg[7] = (int) sht;
	} else if (edx == 6) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		wordsDraw8(sht->buf, sht->bxsize, esi, edi, eax, (char *) ebp + fileBase);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
		}
	} else if (edx == 7) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		boxDraw8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi);
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
		}
	} else if (edx == 8) {
		initMemeryManager((MEMERY_LIST *) (ebx + fileBase));
		ecx &= 0xfffffff0;
		memeryFree4k((MEMERY_LIST *) (ebx + fileBase), eax, ecx);
	} else if (edx == 9) {
		ecx = (ecx + 0x0f) & 0xfffffff0; 
		reg[7] = memeryAlloc((MEMERY_LIST *) (ebx + fileBase), ecx);
	} else if (edx == 10) {
		ecx = (ecx + 0x0f) & 0xfffffff0; 
		memeryFree4k((MEMERY_LIST *) (ebx + fileBase), eax, ecx);
	} else if (edx == 11) {
		sht = (struct SHEET *) (ebx & 0xfffffffe);
		sht->buf[sht->bxsize * edi + esi] = eax;
		if ((ebx & 1) == 0) {
			sheet_refresh(sht, esi, edi, esi + 1, edi + 1);
		}
	} else if (edx == 12) {
		sht = (struct SHEET *) ebx;
		sheet_refresh(sht, eax, ecx, esi, edi);
	}
	return 0;
} */

/* void sysinfoCMD(CONSOLE *console){
	char info[32];
	printLine(console);		
	sprintf(info, "File_System:%s",NNOS_FILE_SYSTEM);  //输出文件系统信息
	labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
	//newCMDLine(console);
	sprintf(info, "Version:%s",NNOS_VERSION);  //输出系统版本信息
	labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
	//newCMDLine(console);
	sprintf(info, "Based:%s",NNOS_CPU_BASED);  //输出CPU架构信息
	labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息						
	//newCMDLine(console);
	sprintf(info, "Campany:%s",NNOS_COMPANY);  //输出公司信息
	labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息							
	//newCMDLine(console);
	sprintf(info, "Author:%s",NNOS_AUTHOR);  //输出作者信息
	labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息						
	//newCMDLine(console);	
	sprintf(info, "Email:%s", NNOS_EMAIL);  //输出开发者邮箱信息
	sysprintl(console,info);
	//labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息							
	//newCMDLine(console);
	sprintf(info, "Update_Date:%s",NNOS_UPDATE_DATE);  //输出升级日期信息
	sysprintl(console,info);
	//labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息	
	//newCMDLine(console);
	printLine(console);		
} */

/* void newCMDLine(CONSOLE *console){
	int x,y; //元素坐标
	if (console->cursorY <  CONSOLE_TEXT_HEIGHT - 36){
	//if(console->cursorY <= CONSOLE_TEXT_HEIGHT - 52){
		console->cursorY += 16;
	}
	else{
		for(y = WINDOW_CAPTION_HEIGHT + 8;y < 28 + CONSOLE_TEXT_HEIGHT;y++){
			for(x = 8;x < CONSOLE_TEXT_WIDTH - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = console->sheet->buf[x + (y + 16) * console->sheet->bxsize];
			}
		}
		for(y = WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_TEXT_HEIGHT;y < WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_TEXT_HEIGHT;y++){
			for(x = 8;x < 8 + CONSOLE_TEXT_WIDTH;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
			}
		}
		//console->sheet->buf[CONSOLE_TEXT_WIDTH + CONSOLE_TEXT_HEIGHT * console->sheet->bxsize] = COL8_D9D9D9;
		sheet_refresh(console->sheet,8,WINDOW_CAPTION_HEIGHT + 8,CONSOLE_TEXT_WIDTH,CONSOLE_TEXT_HEIGHT);
		//sheet_refresh(CONSOLE_TEXT_HEIGHT,8,28,CONSOLE_TEXT_WIDTH - 16,CONSOLE_TEXT_HEIGHT);
	}
} */

/*控制台换行*/
/* void newCMDLine(CONSOLE *console){
	int x,y; //元素坐标
	if (console->cursorY <  CONSOLE_HEIGHT - 36){
	//if(console->cursorY <= CONSOLE_HEIGHT - 52){
		console->cursorY += 16;
	}
	else{
		for(y = WINDOW_CAPTION_HEIGHT + 8;y < 28 + CONSOLE_HEIGHT;y++){
			for(x = 8;x < CONSOLE_WIDTH - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = console->sheet->buf[x + (y + 16) * console->sheet->bxsize];
			}
		}
		for(y = WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_HEIGHT;y < WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_HEIGHT;y++){
			for(x = 8;x < 8 + CONSOLE_WIDTH;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console->sheet,8,WINDOW_CAPTION_HEIGHT + 8,CONSOLE_WIDTH,CONSOLE_HEIGHT);
	}
} */

/* void runCMD(CONSOLE *console,int *fat,char *command){
	int myLen = 4;
	int x,y;
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)GDT_ADR; //定义GDT并指向系统GDT
	FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	char *file;
	x = 0;y = 0; //初始化游标
	int flag = 0;
	char *fileName = (char *) memeryAlloc4k(memeryList,FILE_FULL_NAME_SIZE * sizeof(char));
	char *fileExten = (char *) memeryAlloc4k(memeryList,FILE_EXTEN_SIZE * sizeof(char));
	for(x = myLen;x < myLen + FILE_NAME_SIZE + 1;x++){
		if(command[x] == '.'){
			strncpy(fileName,command + myLen,x - myLen); //解析文件名	
			flag = 1;
			break;
		}
	}
	if(flag == 0){
		strncpy(fileName,command + myLen,FILE_NAME_SIZE + myLen); //解析文件名
		//flag = 1;
	}
	convertToUppercase(fileName); //转为大写
	sprintf(fileName, "%-8s",fileName);
	flag = 0;
	strncpy(fileExten,command + x + 1,3); //解析拓展名
	convertToUppercase(fileExten); //转为大写
	sprintf(fileExten, "%-3s",fileExten);
	if(strcmp(fileExten,"   ") == 0){
		fileExten = "NEX";
	}
	strcat(fileName,fileExten);							
	for(x = 0;x < FILE_INFO_MAX;x++){
		if(fileInfo[x].name[0] == 0x00){ //无文件信息
			break;
		}
		if((fileInfo[x].type & 0x18) == 0){ //权限正常
			for(y = 0;y < FILE_FULL_NAME_SIZE;y++){
				if(fileInfo[x].name[y] == fileName[y]){
					flag = 1;
				}
				else{
					flag = 0;
					break;
				}
			}
			if(flag == 1){									
				break;
			}
		}
	}
	if(flag == 1){
		y = fileInfo[x].size; //文件长度
		file = (char *)memeryAlloc4k(memeryList,fileInfo[x].size); //为文件申请空间
		loadFile(fileInfo[x].cluster,fileInfo[x].size,file,fat,(char *)(0x003e00 + DISK_ADR)); //装载文件
		setSegmDesc(gdt + 1003,fileInfo[x].size - 1,(int)file,CODE32_ER_PRE); //设定应用程序段，3号，1~2号由task.c使用
		farjmp(0,1003 * 8); //执行应用程序
		memeryFree4k(memeryList,(int)file,fileInfo[x].size); //执行完毕释放应用程序所占内存
	}
	else{
		labelDraw(console->sheet,8,console->cursorY,COL8_38CE2F,COL8_000000,"File not fount.",15); //绘制信息							
	}
	newCMDLine(console);
	memeryFree4k(memeryList,(unsigned int)fileName,FILE_FULL_NAME_SIZE * sizeof(char));
	memeryFree4k(memeryList,(unsigned int)fileExten,FILE_EXTEN_SIZE * sizeof(char));	
} */

/* void newCMDLine(CONSOLE *console){
	int x,y; //元素坐标
	if (console->cursorY <  CONSOLE_TEXT_HEIGHT - 36){
	//if(console->cursorY <= CONSOLE_TEXT_HEIGHT - 52){
		console->cursorY += 16;
	}
	else{
		for(y = WINDOW_CAPTION_HEIGHT + 8;y < 28 + CONSOLE_TEXT_HEIGHT;y++){
			for(x = 8;x < CONSOLE_TEXT_WIDTH - 8;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = console->sheet->buf[x + (y + 16) * console->sheet->bxsize];
			}
		}
		for(y = WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_TEXT_HEIGHT;y < WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_TEXT_HEIGHT;y++){
			for(x = 8;x < 8 + CONSOLE_TEXT_WIDTH;x++){
				console->sheet->buf[x + y * console->sheet->bxsize] = COL8_000000;
			}
		}
		//console->sheet->buf[CONSOLE_TEXT_WIDTH + CONSOLE_TEXT_HEIGHT * console->sheet->bxsize] = COL8_D9D9D9;
		sheet_refresh(console->sheet,8,WINDOW_CAPTION_HEIGHT + 8,CONSOLE_TEXT_WIDTH,CONSOLE_TEXT_HEIGHT);
		//sheet_refresh(CONSOLE_TEXT_HEIGHT,8,28,CONSOLE_TEXT_WIDTH - 16,CONSOLE_TEXT_HEIGHT);
	}
	//return console.cursorY;
} */

/* void newCMDLine(CONSOLE *console){
	int x, y;
	SHEET *sheet = console->sheet;
	if (console->cursorY < 28 + 112) {
		console->cursorY += 16; 
	} 
	else {
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	console->cursorX = 8;
	return;
} */

/* int newCMDLine(int console.cursorY,SHEET *console.sheet){
	int x,y; //元素坐标
	if (console.cursorY <  CONSOLE_TEXT_HEIGHT - 36){
	//if(console.cursorY <= CONSOLE_TEXT_HEIGHT - 52){
		console.cursorY += 16;
	}
	else{
		for(y = WINDOW_CAPTION_HEIGHT + 8;y < 28 + CONSOLE_TEXT_HEIGHT;y++){
			for(x = 8;x < CONSOLE_TEXT_WIDTH - 8;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = console.sheet->buf[x + (y + 16) * console.sheet->bxsize];
			}
		}
		for(y = WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_TEXT_HEIGHT;y < WINDOW_CAPTION_HEIGHT + 8 + CONSOLE_TEXT_HEIGHT;y++){
			for(x = 8;x < 8 + CONSOLE_TEXT_WIDTH;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = COL8_000000;
			}
		}
		//console.sheet->buf[CONSOLE_TEXT_WIDTH + CONSOLE_TEXT_HEIGHT * console.sheet->bxsize] = COL8_D9D9D9;
		sheet_refresh(console.sheet,8,WINDOW_CAPTION_HEIGHT + 8,CONSOLE_TEXT_WIDTH,CONSOLE_TEXT_HEIGHT);
		//sheet_refresh(CONSOLE_TEXT_HEIGHT,8,28,CONSOLE_TEXT_WIDTH - 16,CONSOLE_TEXT_HEIGHT);
	}
	return console.cursorY;
} */

/* int newCMDLine(int console.cursorY,SHEET *console.sheet){
	int x,y; //元素坐标
	if(console.cursorY <= CONSOLE_HEIGHT - 32){
		console.cursorY += 16;
	}
	else{
		for(y = 28;y< CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH - 32;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = console.sheet->buf[x + (y + 16) * console.sheet->bxsize];
			}
		}
		for(y = CONSOLE_HEIGHT - 28;y < CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH - 16;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console.sheet,8,28,CONSOLE_WIDTH - 16,CONSOLE_HEIGHT);
	}
	return console.cursorY;
} */

/* int newCMDLine(int console.cursorY,SHEET *console.sheet){
	int x,y; //元素坐标
	if(console.cursorY <= CONSOLE_HEIGHT - 32){
		console.cursorY += 16;
	}
	else{
		for(y = 28;y< CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH - 32;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = console.sheet->buf[x + (y + 16) * console.sheet->bxsize];
			}
		}
		for(y = CONSOLE_HEIGHT - 28;y < CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH - 16;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console.sheet,8,28,CONSOLE_WIDTH - 16,CONSOLE_HEIGHT);
	}
	return console.cursorY;
} */

/* int newCMDLine(int console.cursorY,SHEET *console.sheet){
	int x,y; //元素坐标
	if(console.cursorY <= CONSOLE_HEIGHT - 32){
		console.cursorY += 16;
	}
	else{
		for(y = 28;y< CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH - 32;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = console.sheet->buf[x + (y + 16) * console.sheet->bxsize];
			}
		}
		for(y = CONSOLE_HEIGHT - 28;y < CONSOLE_HEIGHT - 8;y++){
			for(x = 8;x < CONSOLE_WIDTH - 16;x++){
				console.sheet->buf[x + y * console.sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console.sheet,8,28,CONSOLE_WIDTH - 16,CONSOLE_HEIGHT);
	}
	return console.cursorY;
} */

/* int newCMDLine(int console.cursorY, struct SHEET *console.sheet)
{
	int x, y;
	if (console.cursorY <= CONSOLE_HEIGHT - 32) {
		console.cursorY += 16; 
	} else {
		for (y = 28; y < 28 + CONSOLE_HEIGHT - 16; y++) {
			for (x = 8;x < CONSOLE_WIDTH - 16;x++) {
				console.sheet->buf[x + y * console.sheet->bxsize] = console.sheet->buf[x + (y + 16) * console.sheet->bxsize];
			}
		}
		//for(y = CONSOLE_HEIGHT - (WINDOW_CAPTION_HEIGHT + 8);y < 28 + CONSOLE_HEIGHT;y++){
		for (y = 28 + CONSOLE_HEIGHT; y < 28 + CONSOLE_HEIGHT; y++) {
			for (x = 8; x < 8 + CONSOLE_WIDTH - 16; x++) {
				console.sheet->buf[x + y * console.sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(console.sheet, 8, 28, 8 + CONSOLE_WIDTH, 28 + CONSOLE_HEIGHT);
	}
	return console.cursorY;
} */

/* void consoleTask(SHEET *sheet,unsigned int totalFreeSize){
	//FIFO_BUFFER32 FIFOBuffer32;
	TIMER *timer;
	TASK *task = taskNow(); //consoleTask需执行休眠，须知本身的Task所在内存地址，为避免传值，直接使用taskNow获取此地址
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR; //内存表
	SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)GDT_ADR; //定义GDT并指向系统GDT
	int data;
	int buffer[128],cursorX = 16,cursorY = 60,cursorC = -1;
	char info[32],command[32];
	char *file; //文件指针
	int x,y; //元素坐标
	int *fat = (int *)memeryAlloc4k(memeryList,4 * 2880); //为文件分配表申请空间，分配表有两张（一张为备份）
	FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	initFIFOBuffer32(&task->fifo,buffer,128,task);
	timer = timerAlloc();initTimer(timer,&task->fifo,1);setTimer(timer,50); //初始化定时器缓冲区
	readFAT(fat,(unsigned char *)(DISK_ADR + 0X000200)); //载入文件分配表
	
	//cursorY = newCMDLine(cursorY,sheet);
	//cursorY = newCMDLine(cursorY,sheet);
	labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,">",1);
	
	while(1){
		io_cli();
		if(getFIFOBufferStatus32(&task->fifo) == 0){
			taskSleep(task);
			io_sti();
		}
		else{
			data = getFIFOBuffer32(&task->fifo);
			io_sti();
			if(data <= 1){
				if(data != 0){
					initTimer(timer,&task->fifo,0);
					if(cursorC >= 0){
						cursorC = COL8_FFFFFE;
					}
				}
				else{
					initTimer(timer,&task->fifo,1);
					if(cursorC >= 0){
						cursorC = COL8_000000;
					}
				}
				setTimer(timer,50);
			}
			if(data == 2){
				cursorC = COL8_FFFFFF;
			}
			if(data == 3){
				cursorC = - 1;
				boxDraw8(sheet->buf,sheet->bxsize,cursorX,cursorY ,cursorX + 7,43,COL8_000000); //绘制光标
			}
			if(256 <= data && data <=511){
				if(data == 8 + 256){ //删除键
					if(cursorX > 16){
						labelDraw(sheet,cursorX,cursorY,COL8_FFFFFE,COL8_000000," ",1); //输出字符
						cursorX -= 8;
					}
				}
				else if(data == 10 + 256){ //回车键
					labelDraw(sheet,cursorX,cursorY,COL8_FFFFFE,COL8_000000," ",1); //输出字符
					command[cursorX / 8 - 2] = 0; //空出两个保存按键字符		
					cursorY = newCMDLine(cursorY,sheet);
					//if(command[0] == 'm' && command[1] == 'e' && command[2] == 'm' && command[3] ==0){ //mem,内存查询
					if(strcmp(command,"mem" ) == 0 || strcmp(command,"free") == 0){ //mem,内存查询
						sprintf(info, "Memery:%dMB", (memeryList->total_size / (1024 * 1024)));  //输出内存总量信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);
						sprintf(info,"Free:%dKB",(memeryList->total_free_size / 1024)); //输出空闲内存信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);
						//cursorY = newCMDLine(cursorY,sheet);				
					}
					else if(strcmp(command,"cls") == 0 || strcmp(command,"clear") == 0){ //cls,清屏
						for(y = 8;y < CONSOLE_HEIGHT - 2;y++){
							for(x = 8;x < CONSOLE_WIDTH - 16;x++){
								sheet->buf[x + y * sheet->bxsize] = COL8_000000;
							}
						}
						sheet_refresh(sheet,8,28,CONSOLE_WIDTH - 16,CONSOLE_HEIGHT - 32);
						cursorY = 28;
					}
					else if(strcmp(command,"version") == 0){ //version,版本信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,strcat("version:",NNOS_VERSION),sizeof(NNOS_VERSION) + 7); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);
					}
					else if(strcmp(command,"sysinfo") == 0 || strcmp(command,"osinfo") == 0 || strcmp(command,"nnos") == 0){ //sysinfo,系统信息
						sprintf(info, "File_System:%s",NNOS_FILE_SYSTEM);  //输出文件系统信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);
						sprintf(info, "Version:%s",NNOS_VERSION);  //输出系统版本信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);
						sprintf(info, "Based:%s",NNOS_CPU_BASED);  //输出CPU架构信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息						
						cursorY = newCMDLine(cursorY,sheet);
						sprintf(info, "Campany:%s",NNOS_COMPANY);  //输出公司信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息							
						cursorY = newCMDLine(cursorY,sheet);
						sprintf(info, "Author:%s",NNOS_AUTHOR);  //输出作者信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息						
						cursorY = newCMDLine(cursorY,sheet);	
						sprintf(info, "Email:%s", NNOS_EMAIL);  //输出开发者邮箱信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息							
						cursorY = newCMDLine(cursorY,sheet);
						sprintf(info, "Update_Date:%s",NNOS_UPDATE_DATE);  //输出升级日期信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息							
						cursorY = newCMDLine(cursorY,sheet);						
					}
					else if(strcmp(command,"dir") == 0 || strcmp(command,"ls") == 0){ //dir,磁盘信息
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,"[filename]  [exten]   [size]",32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);	
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,"-----------------------------",32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);	
						for(x = 0;x < FILE_INFO_MAX;x++){ //无文件信息（首字节为0x00，无文件信息）
							if(fileInfo[x].name[0] == 0x00){
								break;
							}
							if(fileInfo[x].name[0] != 0xe5){ //文件未删除（首字节为0xe5，文件已删除）
								if((fileInfo[x].type & 0x18) == 0){ //文件属性值正常，文件信息，非目录
									char fileName[9];
									//char *fileName = (char *) memeryAlloc4k(memeryList,9 * sizeof(char));
									strncpy(fileName,fileInfo[x].name,8);
									sprintf(info,"%s    .%s   %7d",fileName,fileInfo[x].exten,fileInfo[x].size);
									labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
									cursorY = newCMDLine(cursorY,sheet);																
								}								
							}
						}
						//memeryFree(memeryList,fileName,9 * sizeof(char));
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,"-----------------------------",32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);						
					}
					else if(strncmp(command,"type ",5) == 0 || strncmp(command,"cat ",4) == 0){
						int myLen = 5;
						if(strncmp(command,"cat ",4) == 0){
							myLen = 4;
						}
						x = 0;y = 0; //初始化游标
						int flag = 0;
						//char fileName[FILE_FULL_NAME_SIZE];
						//char fileExten[FILE_EXTEN_SIZE];
						char *fileName = (char *) memeryAlloc4k(memeryList,FILE_FULL_NAME_SIZE * sizeof(char));
						char *fileExten = (char *) memeryAlloc4k(memeryList,FILE_EXTEN_SIZE * sizeof(char));
						for(x = myLen;x < myLen + FILE_NAME_SIZE + 1;x++){
							if(command[x] == '.'){
								strncpy(fileName,command + myLen,x - myLen); //解析文件名	
								flag = 1;
								break;
							}
						}
						if(flag == 0){
							strncpy(fileName,command + myLen,FILE_NAME_SIZE + myLen); //解析文件名
							//flag = 1;
						}
						convertToUppercase(fileName); //转为大写
						sprintf(fileName, "%-8s",fileName);
						flag = 0;
						strncpy(fileExten,command + x + 1,3); //解析拓展名
						convertToUppercase(fileExten); //转为大写
						sprintf(fileExten, "%-3s",fileExten);
						strcat(fileName,fileExten);
						//labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,fileName,32); //绘制信息
						//cursorY = newCMDLine(cursorY,sheet);							
						for(x = 0;x < FILE_INFO_MAX;x++){
							if(fileInfo[x].name[0] == 0x00){ //无文件信息
								break;
							}
							if((fileInfo[x].type & 0x18) == 0){ //权限正常
								for(y = 0;y < FILE_FULL_NAME_SIZE;y++){
									if(fileInfo[x].name[y] == fileName[y]){
										flag = 1;
									}
									else{
										flag = 0;
										break;
									}
								}
								if(flag == 1){									
									break;
								}
							}
						}
						if(flag == 1){
							y = fileInfo[x].size; //文件长度
							//file = (char *) (fileInfo[x].cluster * 512 + 0x003e00 + DISK_ADR); //文件首地址
							file = (char *)memeryAlloc4k(memeryList,fileInfo[x].size); //为文件申请空间
							loadFile(fileInfo[x].cluster,fileInfo[x].size,file,fat,(char *)(0x003e00 + DISK_ADR)); //装载文件
							cursorX = 16; //复位光标
							for(x = 0;x < y;x++){ //遍历文件内容
								info[0] = file[x]; //逐个打印字符
								info[1] = '\0';
								if(info[0] == 0x09){ //制表符
									while(1){ //寻找制表位，4的倍数
										labelDraw(sheet,8 + x * 8,cursorY,COL8_38CE2F,COL8_000000," ",1); //填充空格
										cursorX += 8; //光标后移
										if(cursorX == CONSOLE_TEXT_WIDTH - 8){ //换行
											cursorX = 8;
											cursorY = newCMDLine(cursorY,sheet);
										}
										if(((cursorX - 8) & 0x1f) == 0){ //可以被32整除(公因数为4、8、16、32，制表位)，32位恰可用二进制表示，故可直接使用&计算余数
											//size = (size + 0xfff) & 0xfffff000; //以4K为最小单位向上取整。相当于if((i & 0xfff) != 0){i = (i & 0xfffff000) + 0x1000;}
											break; //制表位找到，停止移动光标
										}
									}
								}
								else if(info[0] == 0x0a){ //换行符，换行回车一起处理，对0x0d回车符不再作处理
									cursorX = 8;
									cursorY = newCMDLine(cursorY,sheet);
								}
								else if(info[0] == 0x0d){ //回车符，换行回车一起处理，对0x0d回车符不再作处理
									//不作处理
								}
								else{ //普通字符
									labelDraw(sheet,8 + x * 8,cursorY,COL8_38CE2F,COL8_000000,info,1); //绘制信息
									//cursorY = newCMDLine(cursorY,sheet); //换行
									cursorX += 8; //光标后移
									if(cursorX == CONSOLE_TEXT_WIDTH ){ //最右端换行
										cursorX = 8;
										cursorY = newCMDLine(cursorY,sheet);
									}									
								}
							}
							cursorY = newCMDLine(cursorY,sheet);
							memeryFree4k(memeryList,(int)file,fileInfo[x].size);
						}
						else{
							labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,"File not fount.",15); //绘制信息							
						}
						cursorY = newCMDLine(cursorY,sheet);
						memeryFree4k(memeryList,(unsigned int)fileName,FILE_FULL_NAME_SIZE * sizeof(char));
						memeryFree4k(memeryList,(unsigned int)fileExten,FILE_EXTEN_SIZE * sizeof(char));
					}
					else if(strncmp(command,"run ",4) == 0){
					//else if(strcmp(command,"run ") == 0){
						int myLen = 4;
						x = 0;y = 0; //初始化游标
						int flag = 0;
						char *fileName = (char *) memeryAlloc4k(memeryList,FILE_FULL_NAME_SIZE * sizeof(char));
						char *fileExten = (char *) memeryAlloc4k(memeryList,FILE_EXTEN_SIZE * sizeof(char));
						for(x = myLen;x < myLen + FILE_NAME_SIZE + 1;x++){
							if(command[x] == '.'){
								strncpy(fileName,command + myLen,x - myLen); //解析文件名	
								flag = 1;
								break;
							}
						}
						if(flag == 0){
							strncpy(fileName,command + myLen,FILE_NAME_SIZE + myLen); //解析文件名
							//flag = 1;
						}
						convertToUppercase(fileName); //转为大写
						sprintf(fileName, "%-8s",fileName);
						flag = 0;
						strncpy(fileExten,command + x + 1,3); //解析拓展名
						convertToUppercase(fileExten); //转为大写
						sprintf(fileExten, "%-3s",fileExten);
						//labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,fileExten,32); //绘制信息
						//cursorY = newCMDLine(cursorY,sheet);	
						if(strcmp(fileExten,"   ") == 0){
							fileExten = "NEX";
						}
						strcat(fileName,fileExten);							
						for(x = 0;x < FILE_INFO_MAX;x++){
							if(fileInfo[x].name[0] == 0x00){ //无文件信息
								break;
							}
							if((fileInfo[x].type & 0x18) == 0){ //权限正常
								for(y = 0;y < FILE_FULL_NAME_SIZE;y++){
									if(fileInfo[x].name[y] == fileName[y]){
										flag = 1;
									}
									else{
										flag = 0;
										break;
									}
								}
								if(flag == 1){									
									break;
								}
							}
						}
						if(flag == 1){
							y = fileInfo[x].size; //文件长度
							file = (char *)memeryAlloc4k(memeryList,fileInfo[x].size); //为文件申请空间
							loadFile(fileInfo[x].cluster,fileInfo[x].size,file,fat,(char *)(0x003e00 + DISK_ADR)); //装载文件
							setSegmDesc(gdt + 1003,fileInfo[x].size - 1,(int)file,CODE32_ER_PRE); //设定应用程序段，3号，1~2号由task.c使用
							farjmp(0,1003 * 8); //执行应用程序
							memeryFree4k(memeryList,(int)file,fileInfo[x].size); //执行完毕释放应用程序所占内存
						}
						else{
							labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,"File not fount.",15); //绘制信息							
						}
						cursorY = newCMDLine(cursorY,sheet);
						memeryFree4k(memeryList,(unsigned int)fileName,FILE_FULL_NAME_SIZE * sizeof(char));
						memeryFree4k(memeryList,(unsigned int)fileExten,FILE_EXTEN_SIZE * sizeof(char));
					}
					else if(command[0] != 0){ //非法输入
						//if(sizeof(command) < (sizeof(info) - sizeof(" is invalid command."))){
						//if(command[8]  == 0){
						if(strlen(command)  < 12){
							sprintf(info,"\'%s\' is invalid command.",command); //输出信息
							//command[8] = 0;
						}
						else{
							char commandSub[6],commandSub2[4];
							strncpy(commandSub,command,5);
							strncpy(commandSub2,command+(strlen(command) - 3),3);
							commandSub[5] = '\0';
							commandSub2[3] = '\0';
							sprintf(info,"\'%s...%s\' is invalid command.",commandSub,commandSub2); //输出信息
							//sprintf(info,"\'%s...\' is invalid command.",commandSub); //输出信息
						}
						labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,info,32); //绘制信息
						cursorY = newCMDLine(cursorY,sheet);
					}
					labelDraw(sheet,8,cursorY,COL8_38CE2F,COL8_000000,">",1);
					cursorX = 16;					
				}				
				else{
					if(cursorX < CONSOLE_WIDTH - 16){
						info[0] = data - 256;
						info[1] = 0;
						command[cursorX / 8 - 2] = data - 256;
						labelDraw(sheet,cursorX,cursorY,COL8_38CE2F,COL8_000000,info,1); //擦除
						cursorX += 8;
					}
				}
			}
			if(cursorC >= 0){
				boxDraw8(sheet->buf,sheet->bxsize,cursorX,cursorY,cursorX + 7,cursorY + 15,cursorC); //绘制光标
			}
			sheet_refresh(sheet,cursorX,cursorY,cursorX + 8,cursorY + 16);		
		}
	}
} */
