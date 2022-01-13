#define WIDTH 150
#define HEIGHT 50

#include "api.h"

/* int api_window(char *buffer,int width,int height,int vcolor,char *caption);
void api_ascwin(int win,int x,int y,int vcolor,int len,char *str);
void api_boxwin(int win,int x0,int y0,int x1,int y1,int vcolor);
void api_initmalloc();
char *api_malloc(int size);
void api_return(); */

//char buffer[WIDTH * HEIGHT];
//char buffer[150 * 80];

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

void NNOSMain(void)
{
	char *buf;
	int win;
	api_initmalloc();
	buf = api_malloc(WIDTH * HEIGHT);
	win = api_window(buf, WIDTH,HEIGHT, -1, "API_WIN");
	api_boxwin(win,  8, 24, 141, 42, 1);
	api_ascwin(win, 28, 28, 4 , 12, "Hello world!");
	while(api_getkey(1) != 0x0a);
	api_return();
}

/* void NNOSMain(void)
{
	char *buf;
	int win;
	api_initmalloc();
	buf = api_malloc(WIDTH * HEIGHT);
	win = api_window(buf, WIDTH,HEIGHT, -1, "API_WIN");
	api_boxwin(win,  8, 36, 141, 43, 1);
	api_ascwin(win, 28, 28, 4 , 12, "Hello world!");
	while(api_getkey(1) != 0x0a);
	api_return();
} */

 
/* int api_window(char *buffer,int xsiz,int ysiz,int col_inv,char *title);
void api_ascwin(int window,int x,int y,int col,int len,char *str);
void api_boxwin(int window,int x0,int y0,int x1,int y1,int col);
void api_initmalloc();
char *api_malloc(int size);
void api_return();

void NNOSMain()
{
	char *buffer;
	int window;

	api_initmalloc();
	//buffer = api_malloc(150 * 50);
	buffer = api_malloc(150 * 80);
	//window = api_window(buffer, 150, 50, -1, "hello");
	 window = api_window(buffer,150,80,-1,"API_WIN");
	//api_boxwin(window,  8, 36, 141, 43, 6);
	api_boxwin(window,8,28,141,53,1);
	//api_ascwin(window, 28, 28, 0, 12, "hello, world");
	api_ascwin(window,28,28,4,12,"Hello world!");
	api_return();
}
 */
 