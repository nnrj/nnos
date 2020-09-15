#include "api.h"

#define WIDTH 150
#define HEIGHT 100
 
/* int api_window(char *buffer, int xsiz, int ysiz, int vcolor, char *title);
void api_boxwin(int win, int x0, int y0, int x1, int y1, int vcolor);
void api_initmalloc();
char *api_malloc(int size);
void api_point(int win, int x, int y, int vcolor);
void api_refreshwin(int win,int x0,int y0,int x1,int y1);
void api_return(); */

int rand();

void NNOSMain()
{
	char *buffer;
	int win,i,x,y;
	api_initmalloc();
	buffer = api_malloc(WIDTH * HEIGHT);
	win = api_window(buffer,WIDTH,HEIGHT,-1,"Points");
	api_boxwin(win,6,26,143,93,0);
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) + 6;
		y = (rand() %  67) + 26;
		api_point(win + 1,x,y,4); //窗口句柄偏移1，变为奇数，不刷新图层
	}
	api_refreshwin(win,6,26,144,94); //统一刷新图层
	while(api_getkey(1) != 0x0a);
	api_return();
}

/* int api_window(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_boxwin(int win, int x0, int y0, int x1, int y1, int col);
void api_initmalloc(void);
char *api_malloc(int size);
void api_point(int win, int x, int y, int col);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_return(void);

int rand(void);	

void NNOSMain(void)
{
	char *buf;
	int win, i, x, y;
	api_initmalloc();
	buf = api_malloc(150 * 100);
	win = api_window(buf, 150, 100, -1, "stars2");
	api_boxwin(win + 1,  6, 26, 143, 93, 0);
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) +  6;
		y = (rand() %  67) + 26;
		api_point(win + 1, x, y, 3 );
	}
	api_refreshwin(win,  6, 26, 144, 94);
	api_return();
} */


