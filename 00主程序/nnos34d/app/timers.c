#include <stdio.h>
#include "api.h"
#define WIDTH 150
#define HEIGHT 80
/* int api_window(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_ascwin(int win, int x, int y, int col, int len, char *str);
void api_boxwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
char *api_malloc(int size);
int api_getkey(int mode);
int api_alloctimer(void);
void api_inittimer(int timer, int data);
void api_settimer(int timer, int time);
void api_return(void); */

void NNOSMain()
{
	char *buf, s[12];
	int win, timer, sec = 0, min = 0, hou = 0;
	api_initmalloc();
	buf = api_malloc(150 * 50);
	win = api_window(buf, 150, 50, -1, "Timers");
	timer = api_alloctimer();
	api_inittimer(timer, 128);
	while(1){
		sprintf(s, "%5d:%02d:%02d", hou, min, sec);
		api_boxwin(win, 28, 27, 115, 41, 7);
		api_ascwin(win, 28, 27, 0, 11, s);
		api_settimer(timer, 100);
		if (api_getkey(1) != 128) {
			break;
		}
		sec++;
		if (sec == 60) {
			sec = 0;
			min++;
			if (min == 60) {
				min = 0;
				hou++;
			}
		}
	}
	//while(api_getkey(1) != 0x0a);
	api_return();
}

/* void NNOSMain(){
	char *buffer;
    int window;
	api_initmalloc();
	buffer = api_malloc(WIDTH * HEIGHT);
    window = api_window(buffer,WIDTH,HEIGHT,-1,"API_WIN");
	api_boxwin(window,8,28,141,53,1);
	api_ascwin(window,28,28,4,12,"Hello world!");
	while(api_getkey(1) != 0x0a);
    api_return();
} */
