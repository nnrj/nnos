/**
 * ===================NNOS-graphics===========================
 *【系统服务相关函数】【应用层】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

void tssBMain(SHEET *sht_win_b)
{
	FIFO_BUFFER32 FIFOBuffer32;
	TIMER *putTimer,*lsTimer; //测试用
	int data;
	int fifobuf[128];
	char info[12];
	COUNT count = 0;
	COUNT count0 = 0; //测试用
	initFIFOBuffer32(&FIFOBuffer32,fifobuf,128,0);
	putTimer = timerAlloc();
	initTimer(putTimer,&FIFOBuffer32,1);
	setTimer(putTimer,1);	
	lsTimer = timerAlloc();
	initTimer(lsTimer,&FIFOBuffer32,100);
	setTimer(lsTimer,100); //测试用
	

	while(1){
		count++;
		io_cli();
		if(getFIFOBufferStatus32(&FIFOBuffer32) == 0){
			//io_stihlt();
			io_sti(); //测试用，CPU全速运行
		}
		else{
			data = getFIFOBuffer32(&FIFOBuffer32);
			io_sti();
			if(data == 100){
				sprintf(info,"%11d",count - count0);
				labelDraw(sht_win_b,16,WINDOW_CAPTION_HEIGHT + 8,COL8_000000,COL8_FFFFFE,info,11);
				count0 = count;
				setTimer(lsTimer,100);
			}
			else if(data == 1){
				sprintf(info,"%11d",count);
				labelDraw(sht_win_b,16,WINDOW_CAPTION_HEIGHT + 30,COL8_000000,COL8_FFFFFE,info,11);		
				setTimer(putTimer,1);				
			}
		}
	}
}
