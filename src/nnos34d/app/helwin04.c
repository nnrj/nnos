#define WIDTH 150
#define HEIGHT 50

#include "api.h"

void NNOSMain(){
	//char *buffer;
	char buffer[WIDTH * HEIGHT];
	int window;
	api_initmalloc();
	//buffer = api_malloc(WIDTH * HEIGHT);
	window = api_window(buffer,WIDTH,HEIGHT, -1,"API_WIN");
	api_boxwin(window,8,24,141,42,1);
	api_ascwin(window,28,28,4,12,"Hello world!");
	while(api_getkey(1) != 0x0a);
	api_return();
}
