#include "api.h"

#define WIDTH 160
#define HEIGHT 100

/* int api_window(char *buffer, int xsiz, int ysiz, int col_inv, char *title);
void api_boxwin(int window, int x0, int y0, int x1, int y1, int vcolor);
void api_initmalloc();
char *api_malloc(int size);
void api_point(int window, int x, int y, int vcolor);
void api_refreshwin(int window,int x0,int y0,int x1,int y1);
void api_linewin(int window,int x0,int y0,int x1,int y1, int vcolor);
//void api_closewin(int window);
void api_return();
 */
void NNOSMain(){
	char *buffer;
	int window, i;
	api_initmalloc();
	buffer = api_malloc(WIDTH * HEIGHT);
	window = api_window(buffer,160,100,-1,"Lines");
	for(i = 0;i < 8;i++){
		api_linewin(window+1,8,26,77,i * 9 + 26,i);
		api_linewin(window+1,88,26,i * 9 + 88,89,i);
	}
	api_refreshwin(window,6,26,154,90);
	while(api_getkey(1) != 0x0a);
	//while(api_getkey(1) != 0x13); //按ESC退出
	api_closewin(window);
	api_return();
}
