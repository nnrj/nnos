#include "api.h"

void NNOSMain()
{
	int win;
	char buf[150 * 70];
	win = api_window(buf, 150, 70, 255, "Anbox");
	api_boxwin(win,   0, 50,  34, 69, 255);
	api_boxwin(win, 115, 50, 149, 69, 255);
	api_boxwin(win,  50, 30,  99, 49, 255);
	
	while(api_getkey(1) != 0x0a); //Enter键结束运行
	
	api_return();
}
