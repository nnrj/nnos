/**
 * ===================NNOS-graphics===========================
 *【应用程序接口（API）】【基础层】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/

/*常量*/
#define API_VERSION "0.34d"
#define COLOR_BASE 27

/*字符输出*/
void api_printc(int c); //输出单个字符
void api_printl(char *s); //绘制字符串

/*图形绘制*/
void api_boxwin(int win,int x0,int y0,int x1,int y1,int vcolor); //画色块（兼容）
//void api_boxwin(int win,int x0,int y0,int x1,int y1,int vcolor);
void api_point(int win,int x,int y,int vcolor); //画点

/*窗口相关*/
int api_window(char *buffer,int width,int height,int vcolor,char *caption); //窗口创建
void api_ascwin(int win,int x,int y,int vcolor,int len,char *str); //在窗口上绘制字符串
void api_linewin(int win,int x0,int y0,int x1,int y1, int vcolor); //画线
void api_refreshwin(int win, int x0, int y0, int x1, int y1); //窗口刷新
void api_closewin(int win); //关闭窗口

/*内存相关*/
void api_initmalloc(); //初始化内存
char *api_malloc(int size); //申请内存
void api_free(char *addr, int size); //释放内存

/*控制相关*/
void api_return(); //返回调用者

/*输入相关*/
int api_getkey(int mode);

/*定时器相关*/
int api_alloctimer();
void api_inittimer(int timer, int data);
void api_settimer(int timer, int time);
void api_freetimer(int timer);

/*蜂鸣发声器相关*/
void api_beep(int tone);

/*文件处理相关*/
int api_fopen(char *fileName); //打开文件
void api_fclose(int fileHanle); //关闭文件
void api_fseek(int fileHanle, int offset, int mode); //定位文件
int api_fsize(int fileHanle, int mode); //获取文件大小
int api_fread(char *buffer, int maxsize, int fileHanle); //读取文件

/*控制台相关*/
int api_command(char *buffer, int maxSize); //获取控制台输入

/*字体相关*/
int api_getlang();

/*标准函数库*/
int putchar(int c);
int exit(int status);
int printf(char *format,...);
void *malloc(int size);
void free(void *p);
void scanf(char *format,...);

