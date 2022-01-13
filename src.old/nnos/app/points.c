/* int api_window(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_boxwin(int win, int x0, int y0, int x1, int y1, int vcolor);
void api_initmalloc();
char *api_malloc(int size);
void api_point(int win, int x, int y, int vcolor);
void api_return();

void NNOSMain()
{
	char *buf;
	int win;
	api_initmalloc();
	buf = api_malloc(150 * 100);
	win = api_window(buf,150,100,-1,"Point");
	api_boxwin(win,6,26,143,93,0);
	api_point(win,75,59,3);
	api_return();
}
 */
#define WIDTH 150
#define HEIGHT 100
 
int api_window(char *buffer, int xsiz, int ysiz, int col_inv, char *title);
void api_boxwin(int win, int x0, int y0, int x1, int y1, int vcolor);
void api_initmalloc();
char *api_malloc(int size);
void api_point(int win, int x, int y, int vcolor);
void api_return();

int rand();

void NNOSMain()
{
	char *buffer;
	int win,i,x,y;
	api_initmalloc();
	buffer = api_malloc(150 * 100);
	win = api_window(buffer,WIDTH,HEIGHT,-1,"Points");
	api_boxwin(win,6,26,143,93,0);
	for (i = 0; i < 50; i++) {
		x = (rand() % 137) + 6;
		y = (rand() %  67) + 26;
		api_point(win,x,y,4);
	}
	api_return();
}



