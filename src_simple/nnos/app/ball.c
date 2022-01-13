#include "api.h"


void NNOSMain(void)
{
	int win, i, j, dis;
	char buf[216 * 237];
	struct POINT {
		int x, y;
	};
	static struct POINT table[16] = {
		{ 204, 129 }, { 195,  90 }, { 172,  58 }, { 137,  38 }, {  98,  34 },
		{  61,  46 }, {  31,  73 }, {  15, 110 }, {  15, 148 }, {  31, 185 },
		{  61, 212 }, {  98, 224 }, { 137, 220 }, { 172, 200 }, { 195, 168 },
		{ 204, 129 }
	};

	win = api_window(buf, 216, 237, -1, "bball");
	api_boxwin(win, 8, 29, 207, 228, 0);
	for (i = 0; i <= 14; i++) {
		for (j = i + 1; j <= 15; j++) {
			dis = j - i; 
			if (dis >= 8) {
				dis = 15 - dis; 
			}
			if (dis != 0) {
				api_linewin(win, table[i].x, table[i].y, table[j].x, table[j].y, 8 - dis);
			}
		}
	}

	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break; 
		}
	}
	api_return();
}




/* #include "api.h"

typedef struct POINT{
	int x, y;
}POINT;

void NNOSMain(){
	int win, i, j, dis;
	char buf[216 * 237];
	
	static POINT table[16] = {
		{204,129},{195,90},{172,58},{137,38},{98,34},
		{61,46},{31,73},{15,110},{15,148},{31,185},
		{61,212},{98,224},{137,220},{172,200},{195,168},
		{204,129}
	};

	win = api_window(buf, 216, 237, -1, "Lines02");
	api_boxwin(win, 8, 29, 207, 228, 0);
	
	for(i = 0; i <= 14; i++){
		for (j = i + 1; j <= 15; j++) {
			dis = j - i;
			if (dis >= 8) {
				dis = 15 - dis;
			}
			if (dis != 0) {
				api_linewin(win, table[i].x, table[i].y, table[j].x, table[j].y, 8 - dis);
			}
		}
	}

	while(1){
		if (api_getkey(1) == 0x0a) {
			break; 
		}
	}
	api_return();
} */

/* void NNOSMain(){
	int win, i, j, dis;
	char buf[216 * 237];
	
	struct POINT {
		int x, y;
	};
	
	static struct POINT table[16] = {
		{204,129},{195,90},{172,58},{137,38},{98,34},
		{61,46},{31,73},{15,110},{15,148},{31,185},
		{61,212},{98,224},{137,220},{172,200},{195,168},
		{204,129}
	};

	win = api_window(buf, 216, 237, -1, "Lines02");
	api_boxwin(win, 8, 29, 207, 228, 0);
	for(i = 0; i <= 14; i++){
		for (j = i + 1; j <= 15; j++) {
			dis = j - i;
			if (dis >= 8) {
				dis = 15 - dis;
			}
			if (dis != 0) {
				api_linewin(win, table[i].x, table[i].y, table[j].x, table[j].y, 8 - dis);
			}
		}
	}

	while(1){
		if (api_getkey(1) == 0x0a) {
			break; 
		}
	}
	api_return();
} */

 
/*  #include "api.h"

void NNOSMain(void)
{
	int win, i, j, dis;
	char buf[216 * 237];
	struct POINT {
		int x, y;
	};
	static struct POINT table[16] = {
		{ 204, 129 }, { 195,  90 }, { 172,  58 }, { 137,  38 }, {  98,  34 },
		{  61,  46 }, {  31,  73 }, {  15, 110 }, {  15, 148 }, {  31, 185 },
		{  61, 212 }, {  98, 224 }, { 137, 220 }, { 172, 200 }, { 195, 168 },
		{ 204, 129 }
	};

	win = api_window(buf, 216, 237, -1, "bball");
	api_boxwin(win, 8, 29, 207, 228, 0);
	for (i = 0; i <= 14; i++) {
		for (j = i + 1; j <= 15; j++) {
			dis = j - i; 
			if (dis >= 8) {
				dis = 15 - dis; 
			}
			if (dis != 0) {
				api_linewin(win, table[i].x, table[i].y, table[j].x, table[j].y, 8 - dis);
			}
		}
	}

	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break;  
		}
	}
	api_return();
} */

/* #include "api.h"

#define WIDTH 160
#define HEIGHT 100


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
 */