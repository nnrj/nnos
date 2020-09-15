#include "api.h"

struct DLL_STRPICENV {	//64KB
	int work[64 * 1024 / 4];
};

struct RGB {
	unsigned char b, g, r, t;
};

//bmp.nasm
int info_BMP(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_BMP(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

//jpeg.c
int info_JPEG(struct DLL_STRPICENV *env, int *info, int size, char *fp);
int decode0_JPEG(struct DLL_STRPICENV *env, int size, char *fp, int b_type, char *buf, int skip);

unsigned char rgb2pal(int r, int g, int b, int x, int y);
void error(char *s);

void NNOSMain(void)
{
	struct DLL_STRPICENV env;
	char filebuf[512 * 1024], winbuf[1040 * 805];
	char s[32], *p;
	int win, i, j, fsize, xsize, info[8];
	struct RGB picbuf[1024 * 768], *q;

	api_command(s, 30); //命令行解析
	for (p = s; *p > ' '; p++) { } //读到空格为止
	for (; *p == ' '; p++) { }	//跳过空格

	i = api_fopen(p); if (i == 0) { error("File not found.\n"); } //载入文件
	fsize = api_fsize(i, 0);
	if (fsize > 512 * 1024) {
		error("File too large.\n");
	}
	api_fread(filebuf, fsize, i);
	api_fclose(i);

	if (info_BMP(&env, info, fsize, filebuf) == 0) { //检查文件类型，//非BMP
		if (info_JPEG(&env, info, fsize, filebuf) == 0) { //非JPG
			api_printl("File type unknown.\n");
			api_return();
		}
	}
	/*若以上任何一个info调用成功，则info中包含以下信息：
	info[0] : 文件类型 (1:BMP, 2:JPEG) 
	info[1] : 颜色信息
	info[2] : xsize 
	info[3] : ysize */

	if (info[2] > 1024 || info[3] > 768) {
		error("picture too large.\n");
	}

	xsize = info[2] + 8; //创建窗口
	if (xsize < 136) {
		xsize = 136;
	}
	win = api_window(winbuf, xsize, info[3] + 37, -1, "Picture viwer");

	if (info[0] == 1) { //解析图片文件数据，转换为图像数据
		i = decode0_BMP (&env, fsize, filebuf, 4, (char *) picbuf, 0);
	} else {
		i = decode0_JPEG(&env, fsize, filebuf, 4, (char *) picbuf, 0);
	}
	/*b_byte = 4 表示struct RGB格式
	skip设置为0即可
	*/
	if (i != 0) {
		error("decode error.\n");
	}

	for (i = 0; i < info[3]; i++) { //显示
		p = winbuf + (i + 29) * xsize + (xsize - info[2]) / 2;
		q = picbuf + i * info[2];
		for (j = 0; j < info[2]; j++) {
			p[j] = rgb2pal(q[j].r, q[j].g, q[j].b, j, i);
		}
	}
	api_refreshwin(win, (xsize - info[2]) / 2, 29, (xsize - info[2]) / 2 + info[2], 29 + info[3]);

	for (;;) { //等待结束
		i = api_getkey(1);
		if (i == 'Q' || i == 'q') {
			api_return();
		}
	}
}

unsigned char rgb2pal(int r, int g, int b, int x, int y)
{
	static int table[4] = { 3, 1, 0, 2 };
	int i;
	x &= 1; //判断奇数还是偶数
	y &= 1;
	i = table[x + y * 2];	//生成中间色
	r = (r * 21) / 256; //结果为0~20
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	r = (r + i) / 4;	//结果为0~5
	g = (g + i) / 4;
	b = (b + i) / 4;
	return 27 + r + g * 6 + b * 36;
}

void error(char *s)
{
	api_printl(s);
	api_return();
}
