/**
 * ===================NNOS-window===========================
 *【窗口管理相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/

/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

void initWindowList(WINDOW_LIST *windowList){
	COUNT i;
	windowList = (WINDOW_LIST *)memeryAlloc4k(memeryList,sizeof(WINDOW_LIST)); //为窗口列表申请内存空间
	windowList->number = 0;
	windowList->now = 0;
	for(i = 0;i < 100;i++){
		windowList->autoWindow[i].id = i;
		windowList->autoWindow[i].type = INFO_WINDOW;
		windowList->autoWindow[i].flag = 16;
		windowList->autoWindow[i].X = 16;
		windowList->autoWindow[i].Y = 0;
		windowList->autoWindow[i].width = WINDOW_WIDTH;
		windowList->autoWindow[i].height = WINDOW_HEIGHT;
		windowList->autoWindow[i].caption = "AutoWindow";
		windowList->autoWindow[i].foreColor = COL8_000000;
		windowList->autoWindow[i].backColor = COL8_FFFFFE;
		windowList->autoWindow[i].box = 0;
		windowList->autoWindow[i].visible = 1;
		windowList->autoWindow[i].lock = 0;
		windowList->autoWindow[i].focus = 0;
		windowList->autoWindow[i].flag = 0;	
	}
}

WINDOW *windowAlloc(){
	COUNT i;
	for(i = 0;i < 100;i++){
		if(window[i].flag == 0){
			window[i].flag = 1;
			&return window[i];
		}
	}
	return -1;
}

/* void setWindow(int id){
	COUNT i;
	for(i = 0;i < 100;i++){
		if(i == id){
			window[i].flag = 1;
			&return window[i];
		}
	}	
} */

void setWindowBox(WINDOW_LIST *windowList,WINDOW *window,Box *box){
	window->box = box;
}

void setWindowCaption(WINDOW *window,unsigned char backColor){
	window->box[1].vcolor = backColor;
}

void setWindowFocus(WINDOW_LIST *windowList,WINDOW *window){
	COUNT i;
	WINDOW_LIST windowListBack = (WINDOW_LIST *)memeryAlloc4k(memeryList,sizeof(WINDOW_LIST)); //为窗口列表申请内存空间
	windowListBack = windowList;
	for(i = 0;i < 100;i++){
		if(i != window->id){
			windowList->window[i].focus = 0;
		}
		else{
			windowList->window[i].focus = 1;
			windowList->now = window[i].id;
			return;
		}
	}
	windowList->window[now].focus = 1;
}

WINDOW *getWindow(WINDOW_LIST *windowList,int id){
	COUNT i;
	for(i = 0;i < 100;i++){
		if(i == id){
			return window[i];
		}
	}
	return -1;
}

/* typedef struct WINDOW{
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
	int flag;
}WINDOW; */