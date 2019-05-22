#include "api.h"

#define WIDTH 160
#define HEIGHT 100
#define DE_X WIDTH /2
#define DE_Y HEIGHT / 2 + 10
#define FOOD_NUM 21
#define LINE 5
#define CAPTION 20
#define LINE_CAPTION LINE+CAPTION
#define STEP 8
#define WORD_WIDTH 8
#define WORD_HEIGHT 16
#define DE_LEN 1

typedef struct FOOD{
	int x;
	int y;
	int color;
	int live;
}FOOD;

////void creatFood(FOOD *foodBuffer,int foodNum);

int rand();

void NNOSMain(){
	char *buf;
	//int window,i,x,y,color,len,dis;
	//int window,i,x,y;
	int window,i,x,y,color;
	int flag = 1;
	api_initmalloc();
	buf = api_malloc(WIDTH * HEIGHT);
	window = api_window(buf,WIDTH,HEIGHT,-1,"Snake");
	api_boxwin(window,LINE,LINE_CAPTION,WIDTH - 5,HEIGHT - 5,0);	
	//FOOD *foodBuffer;
	////foodBuffer = (FOOD *)api_malloc(sizeof(FOOD) * FOOD_NUM);
	//FOOD foodBuffer[FOOD_NUM];
	//srand((unsigned)time(NULL));
	for (i = 0; i < FOOD_NUM; i++) {
		x = (rand() % WIDTH - 18) + 6;
		y = (rand() %  HEIGHT - 28) + 26;
		color = (rand() %  25) + 1;
		//api_point(window,x,y,2);
		api_point(window,x,y,color);
	}
	x = DE_X;y = DE_Y;
	//api_point(window,x,y,2);
	////creatFood(foodBuffer,FOOD_NUM);
	////for(i = 0;i < FOOD_NUM;i++){
		//api_point(window,x,y,2);
	////	api_point(window,foodBuffer[i].x,foodBuffer[i].y,foodBuffer[i].color);
	////}
	////i = 0;
	//api_point(window,x,y,color);
	
	api_ascwin(window,x,y,4,DE_LEN,"*");
	while(1 && (flag == 1)){
		i = api_getkey(1);
		api_ascwin(window,x,y,0,1,"*"); 
		switch(i){
			case '4':{
				if(x > LINE + 6){
					x -= STEP;
				}
				break;
			}
			case '6':{
				if(x < WIDTH - WORD_WIDTH - 8){
					x += STEP;
				}
				break;
			}
			case '8':{
				if(y > LINE_CAPTION + 1){
					y -= STEP;
				}
				break;
			}	
			case '2':{
				if(y < HEIGHT - WORD_HEIGHT - LINE - 2){
					y += STEP;
				}
				break;
			}	
			case 0x0a:{
				flag = 0;
				break;
			}			
		}
		//if()
		api_ascwin(window,x,y,4,1,"*");
	}
	api_closewin(window);
	api_return();
}

/* void creatFood(FOOD *foodBuffer,int foodNum){
	int i;
	for (i = 0; i < foodNum; i++) {
		foodBuffer[i].x = (rand() % WIDTH - 18) + 6;
		foodBuffer[i].y = (rand() %  HEIGHT - 28) + 26;
		foodBuffer[i].color = (rand() %  24) + 1;
		foodBuffer[i].live = 1;
	}
} */
