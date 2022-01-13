#include <stdio.h>
#include <stdarg.h>
#include "api.h"
int scanf(char *format,...){ //不定长参数
	int i = 0;
	char c;
	char s[1000];
	va_list arg; //获取参数列表
	int done;
	while(i < 1000){
		c = api_getkey(1);
		if(c != 13){
			s[i] = c;
		}
		else{
			break;
		}
		i++;
	}
	
	va_start(arg, format); //初始化
	done = vsscanf(s,format, arg);
	//api_getkey(1);
	va_end(arg); //扫尾
	return done;
}

/* int printf(char *format,...){ //不定长参数
	va_list ap; //获取参数列表
	char s[1000];
	int i;
	
	va_start(ap,format); //初始化
	i = vsprintf(s,format,ap);
	api_printl(s);
	va_end(ap); //扫尾
	return i;
} */


/* int DEFUN(scanf, (format), CONST char *format DOTS)
{
  va_list arg;
  int done;

  va_start(arg, format);
  done = vscanf(format, arg);
  va_end(arg);

  return done;
} */
