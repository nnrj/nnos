#include <stdio.h>
#include <stdarg.h>
#include "../api.h"

int printf(char *format,...){
	va_list ap;
	char s[1000];
	int i;
	
	va_start(ap,format);
	i = vsprintf(info,forma);
	api_printl(s);
	//api_return();
	va_end(ap);
	return i;
}