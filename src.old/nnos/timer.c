/**
 * ===================NNOS-graphics===========================
 *【定时器相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-21      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

//extern MEMERY_LIST memeryList;
TIMER_LIST timerList; //定时器列表

/*定时器初始化*/
void initTimer(TIMER *timer,FIFO_BUFFER32 *FIFOBuffer,int data){
	timer->fifo = FIFOBuffer;
	timer->data = data;
}

/*定时器分配*/
TIMER *timerAlloc(){
	COUNT i = 0;
	for(i = 0;i < TIMER_MAX;i++){
		if(timerList.timer[i].flag == TIMER_FLAG_UNUSED){
			timerList.timer[i].flag = TIMER_FLAG_USED;
			timerList.timer[i].flag2 = 0;
			return &timerList.timer[i]; //分配
		}
	}
	return NONE; //NONE = 0，未找到
}
 
/*定时器释放*/
void freeTimer(TIMER *timer){
	timer->flag = TIMER_FLAG_UNUSED; //设置定时器状态为未使用
}

/*定时器设定*/
void setTimer(TIMER *timer,unsigned int timeout){
	TIMER *p,*q;
	int eflags;
	timer->timeout = (timerList.count + timeout) % TIMER_TIMER_MAX; //定时器相对时间，从当前时间开始多少秒后
	timer->flag = TIMER_FLAG_USING; //设置定时器状态为正在使用(2)
	eflags = io_load_eflags(); //保护现场，读取标志寄存器，赋值给eflags
	io_cli(); //屏蔽中断，若设定过程中发生中断将造成定时器混乱	
	p = timerList.timers;
	//q = p;
	if(timer->timeout <= p->timeout){
		timerList.timers = timer;
		timer->next = p;
		timerList.next = timer->timeout;
		io_store_eflags(eflags);
		return;
	} 
	while(1){
		q = p;
		p = p->next;
		if(timer->timeout <= p->timeout){
			q->next = timer;
			timer->next = p;
			io_store_eflags(eflags);
			return;
		}
	}
}

/*定时器测试*/
void setTest480(FIFO_BUFFER32 *fifo,int mode){
	COUNT i;
	TIMER *timer;
	if(mode != 0){
		for(i = 0;i < 480;i++){
			timer = timerAlloc();
			initTimer(timer,fifo,1024 + i);
			setTimer(timer,50 * 100 * 24 * 60 * 60 + i * 100);
		}
	}
}

/*关闭定时器*/
int timerCancel(TIMER *timer){
	int e;
	struct TIMER *t;
	e = io_load_eflags(); //保护现场
	io_cli(); //关中断，防止定时器状态改变
	if (timer->flag == TIMER_FLAG_USING){ //判断定时器是否正在运行中
		if (timer == timerList.timers) {
			t = timer->next; //保留第一个定时器
			timerList.timers = t;
			timerList.next = t->timeout;
		}
		else {
			t = timerList.timers; //取消其他定时器
			while(1){ //遍历定时器列表
				if (t->next == timer){
					break;
				}
				t = t->next;
			}
			t->next = timer->next; 
		}
		timer->flag = TIMER_FLAG_USED;
		io_store_eflags(e);
		return 1;
	}
	io_store_eflags(e); //恢复现场
	return 0; 
}

/*关闭所有运行中的定时器*/
void timerCancelAll(FIFO_BUFFER32 *fifo){
	int e,i;
	TIMER *t;
	e = io_load_eflags(); //保护现场
	io_cli(); //关中断，防止定时器状态改变
	for (i = 0;i < TIMER_MAX;i++) {
		t = &timerList.timer[i];
		if (t->flag != TIMER_FLAG_UNUSED && t->flag2 != 0 && t->fifo == fifo) {
			timerCancel(t);
			freeTimer(t);
		}
	}
	io_store_eflags(e); //恢复现场
	return;
}

/*快速时刻调整*/
/* void reTimer(){
	int eflags;
	eflags = io_load_eflags(); //保护现场，读取标志寄存器，赋值给eflags
	io_cli(); //屏蔽中断，若调整过程中发生中断将造成定时器混乱
	timerList.count = timerList.count % TIMER_TIMER_MAX;
	io_store_eflags(eflags); //恢复现场，恢复之前的中断状态	
} */

/*时刻调整*/
/* void reTimer_old(){
	COUNT i;
	int t = timerList.count;
	io_cli();
	timerList.count -= t;
	for(i = 0;i > TIMER_MAX;i++){
		if(timerList.timer[i].flag == TIMER_FLAG_USING){
			timerList.timer[i].timeout -= t;
		}
	}
	io_sti();
} */

void reTImer_old2(){
	COUNT i;
	int t = timerList.count;
	io_cli();
	timerList.count -= t;
	for(i = 0;i > TIMER_MAX;i++){
		if(timerList.timer[i].timeout != TIMER_TIME_MAX_H){
			timerList.timer[i].timeout -= t;
		}
	}
	io_sti();
}

/*定时器队列排序*/
/* void sortTimerList(){
	COUNT i,j;
	i = 0;j = 1;
	TIMER temp;
	for(i = 0;i < TIMER_MAX;i++){
		if(timerList.timer[i].flag == TIMER_FLAG_USING){
			temp = timerList.timer[j];
			timerList.timer[j] = timerList.timer[i];
			timerList.timer[i] = temp;
			j++;
		}	
	}
	for(i = 0;i < timerList.number;i++){
		for(j = i;j < timerList.number;j++){
			if(timerList.timer[i].timeout > timerList.timer[j].timeout){
				temp = timerList.timer[i];
				timerList.timer[i] = timerList.timer[j];
				timerList.timer[j] = temp;
			}
		}
		
	}
} */
