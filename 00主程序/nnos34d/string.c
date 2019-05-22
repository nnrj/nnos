/**
 * ===================NNOS-graphics===========================
 *【字符处理相关函数】【应用层】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

/*转为大写字母*/
void convertToUppercase(char *str){
	char *p;
	p = str;
	while(*p != '\0'){
		if(*p >= 'a' && *p <= 'z'){
			*p -= 0x20;
		}
		p++;
	}
}

/*转为小写字母*/
void convertToLowercase(char *str){
	char *p;
	p = str;
	while(*p != '\0'){
		if(*p >= 'A' && *p <= 'Z'){
			*p += 0x20;
		}
		p++;
	}
}

/* void printLine(){
	labelDraw(console->sheet,8,console->cursorY,CONSOLE_FORECOLOR,COL8_000000,"-----------------------------",32); //绘制信息
	newCMDLine(console);		
} */

/* void printLinex(int len){
	int i;
	for(i = 0;i < len;i++){
		labelDraw(console->sheet,8,console->cursorY,CONSOLE_FORECOLOR,COL8_000000,"-",32); //绘制信息
		newCMDLine(console);		
	}
} */
