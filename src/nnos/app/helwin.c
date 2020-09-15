#define WIDTH 150
#define HEIGHT 80
int api_window(char *buffer,int width,int height,int vcolor,char *caption);
void api_return();

char buffer[WIDTH * HEIGHT];

void NNOSMain(){
    int window;
    window = api_window(buffer,WIDTH,HEIGHT,-1,"API_WIN");
    api_return();
}