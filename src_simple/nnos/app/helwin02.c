#define WIDTH 150
#define HEIGHT 80

int api_window(char *buffer,int width,int height,int vcolor,char *caption);
void api_ascwin(int win,int x,int y,int col,int len,char *str);
void api_boxwin(int win,int x0,int y0,int x1,int y1,int col);
void api_return();

char buffer[WIDTH * HEIGHT];

void NNOSMain(){
    int window = 0;
    window = api_window(buffer,WIDTH,HEIGHT,-1,"API_WIN");
	api_boxwin(window,8,28,141,53,1);
	api_ascwin(window,28,28,4,12,"Hello world!");
    api_return();
}