#include <stdio.h>
#include <stdarg.h>
#include "api.h"


int printf(char *format,...){ //不定长参数
	va_list ap; //获取参数列表
	char s[1000];
	int i;
	
	va_start(ap,format); //初始化
	i = vsprintf(s,format,ap);
	api_printl(s);
	va_end(ap); //扫尾
	return i;
}
