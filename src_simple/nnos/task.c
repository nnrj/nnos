/**
 * ===================NNOS-task===============================
 *【多任务相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-31      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/	
#include "./lib/nnos.h" //系统函数声明

TIMER *taskTimer; //定义任务计时器指针
int taskTR;
TASK_LIST *taskList; //定义任务列表指针

/*任务初始化*/
TASK *initTaskList(MEMERY_LIST *memeryList){
	COUNT i = 0; //计数器
	TASK *task,*idle; //定义任务指针
	SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)GDT_ADR; //定义GDT
	taskList = (TASK_LIST *)memeryAlloc4k(memeryList,sizeof(TASK_LIST)); //为任务列表申请内存空间
	for(i = 0;i < TASK_MAX;i++){
		taskList->tasks0[i].flag =0; //初始化任务列表状态标志
		taskList->tasks0[i].selector = (TASK_GDT0 + i) * 8; //分配段
		taskList->tasks0[i].tss.ldtr = (TASK_GDT0 + TASK_MAX + i) * 8; //将LDT选择子写入tss.ldtr，创建TSS时可设置LDT
		setSegmDesc(gdt + TASK_GDT0 + i,103,(int)&taskList->tasks0[i].tss,TSS32_PRE); //设定段属性
		setSegmDesc(gdt + TASK_GDT0 + TASK_MAX + i, 15, (int)taskList->tasks0[i].ldt,LDT_PRE); //设定段属性
	}
	for(i = 0;i < LEVEL_MAX;i++){ //遍历优先级列表
		taskList->task_level[i].number = 0;
		taskList->task_level[i].now = 0;
	}
	task = taskAlloc(); //为默认任务申请任务表项
	task->flag = TASK_FLAG_RUNNING; //任务正在运行
	task->priority = 2; //第二优先级，时间片0.02秒
	task->level = 0; //最高优先级队列
	addTask(task); //添加任务到优先级队列
	taskSwitchSub(); //决定要切换的任务
	//taskList->number = 1; //初始化时运行中的任务数量为1
	//taskList->now = 0; //当前运行的任务编号为0
	//taskList->tasks[0] = task; //将task赋值给tasks[0]
	load_tr(task->selector); //将任务段选择子装载到tr寄存器
	taskTimer = timerAlloc(); //为任务计时器申请空间
	setTimer(taskTimer,task->priority); //设定任务计时器，0.02秒切换一次

	idle = taskAlloc(); //申请任务项
	idle->tss.esp = memeryAlloc4k(memeryList,64 * 1024) + 64 * 1024;
	idle->tss.eip = (int)&idleTask;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	taskRun(idle,LEVEL_MAX - 1,1); //最低优先级
	
	return task; //将任务指针返回，该默认任务未本任务管理程序
}

/*任务空间分配*/
TASK *taskAlloc(){
	COUNT i;
	TASK *task;
	for(i = 0;i < TASK_MAX;i++){
		if(taskList->tasks0[i].flag == 0){ //遍历定时器列表，寻找空闲表项以供分配
			task = &taskList->tasks0[i];
			task->flag = TASK_FLAG_USED; //设置任务项未使用中，准备就绪
			task->tss.eflags = 0x00000202;
			task->tss.eax = 0;
			task->tss.ebx = 0;
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebp = 0;
			task->tss.edi = 0;
			task->tss.esi = 0;
			task->tss.ds = 0;			
			task->tss.es = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			//task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
			task->tss.ss0 = 0; //运行状态标志位置0
			return task;			
		}
	}
	return 0;
}

/*任务启动*/
void taskRun(TASK *task,int level,int priority){
	if(level < 0){ //指定优先级队列小于0，不合法，不改变
		level = task->level;
	}
	if(priority > 0){ //指定的优先级大于0时改变优先级
		task->priority = priority;
	}
	if(task->flag == TASK_FLAG_RUNNING && task->level != level){
		removeTask(task); //从当前优先级队列移除任务，task->flag变为1
	}
	if(task->flag != TASK_FLAG_RUNNING){
		task->level = level; //改变优先级队列
		addTask(task); //将任务添加到新的优先级队列
	}
	taskList->level_change = 1; //下次任务切换时检查level(??)
}

/*任务手动切换*/
void taskSwitchLimit(){
	switch(taskTR){
		case 3*8:{
			taskTR = 4 * 8;break;
		}
		case 4*8:{
			taskTR = 3 * 8;break;
		}
	}
	setTimer(taskTimer,2);
	farjmp(0,taskTR);
}

/*任务自动切换*/
void taskSwitch(){
	TASK_LEVEL *taskLevel = &taskList->task_level[taskList->level_run];
	TASK *newTask;
	TASK *myTask = taskLevel->tasks[taskLevel->now];
	taskLevel->now++;
	if(taskLevel->now == taskLevel->number){ //防止溢出
		taskLevel->now = 0;
	}
	if(taskList->level_change != 0){
		taskSwitchSub();
		taskLevel = &taskList->task_level[taskList->level_run];
	}
	newTask = taskLevel->tasks[taskLevel->now];
	setTimer(taskTimer,newTask->priority); //设定任务定时器，时间片设置为响应优先级
	if(newTask != myTask){ //两个以上任务才进行切换
		farjmp(0,newTask->selector);
	}
}
 
/*获取当前正在运行的任务*/
TASK *taskNow(){ 
	TASK_LEVEL *taskLevel = &taskList->task_level[taskList->level_run];
	return taskLevel->tasks[taskLevel->now];
}

/*向优先级列表添加任务*/
void addTask(TASK *task){
	TASK_LEVEL *taskLevel = &taskList->task_level[task->level];
	if (taskLevel->number < TASK_LEVEL_MAX){
		taskLevel->tasks[taskLevel->number] = task;
		taskLevel->number++;
		task->flag = TASK_FLAG_RUNNING;		
	}
}

/*从优先级列表移除任务*/
void removeTask(TASK *task){
	COUNT i = 0;
	TASK_LEVEL *taskLevel = &taskList->task_level[task->level];
	for(i = 0;i < taskLevel->number;i++){ //遍历优先级列表，寻找要移除的任务
		if(taskLevel->tasks[i] == task){
			break;
		}
	}
	taskLevel->number--;
	if(i < taskLevel->now){ //移除的任务在当前正在运行的任务之前，移动
		taskLevel->now--;
	}
	if(taskLevel->now >= taskLevel->number){ //溢出
		taskLevel->now = 0;
	}
	task->flag = TASK_FLAG_USED; //转入就绪状态，休眠
	for(;i < taskLevel->number;i++){ //自移除位置开始移动
		taskLevel->tasks[i] = taskLevel->tasks[i+1];
	}
}

/*切换任务子函数*/
void taskSwitchSub(){ //决定要切换的任务
	COUNT i;
	for(i = 0;i < TASK_LEVEL_MAX;i++){ //遍历优先级队列列表
		if(taskList->task_level[i].number > 0){
			break;
		}
	}
	taskList->level_run = i;
	taskList->level_change = 0;
}

/*任务休眠*/
void taskSleep(TASK *task){ //将处于空闲状态的任务从任务列表中移除
	TASK *myTask;
	if(task->flag == TASK_FLAG_RUNNING){
		myTask = taskNow();
		removeTask(task);
		if(task == myTask){ //判断是否自我休眠
			taskSwitchSub(); //自我休眠须先找出需要切换的任务
			myTask = taskNow(); //设定为需要切换的任务
			farjmp(0,myTask->selector); //跳转到该任务
		}
	}
}

/*空闲任务*/
void idleTask(){ //任务优先级队列皆空时，运行空闲任务，使得CPU停机
	while(1){
		io_hlt(); //CPU停机
	}
}
