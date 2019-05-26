/**
 * ===================NNOS-graphics===========================
 *【图形绘制相关】【核心层】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明

/*画板初始化*/
void initPalette(){ //画板初始化函数具体实现，table_rgb见sysdef.h
	setPalette(0,COLORNUM - 1,table_rgb); //调用绘图函数
	unsigned char table2[216 * 3];
	int r, g, b;
	//set_palette(0, 15, table_rgb);
	for (b = 0; b < 6; b++) {
		for (g = 0; g < 6; g++) {
			for (r = 0; r < 6; r++) {
				table2[(r + g * 6 + b * 36) * 3 + 0] = r * 51;
				table2[(r + g * 6 + b * 36) * 3 + 1] = g * 51;
				table2[(r + g * 6 + b * 36) * 3 + 2] = b * 51;
			}
		}
	}
	//setPalette(COLORNUM + 1, 256  - 24 - 1 -(COLORNUM - 16) , table2);
	setPalette(COLORNUM,242, table2);
	//setPalette(16, 231, table2);
}

/*画板设定*/
void setPalette(int start,int end,unsigned char *rgb){	//解析table_rgb，将颜色信息写入到显存
	int i,eflags;
	eflags = io_load_eflags();	//记录中断许可标志值
	io_cli();	//屏蔽中断
	io_out8(0x03c8,start); //向显卡备写入画板号码，通过IN指令从设备取得电气信号，区分不同设备，使用不同的端口号(port)，0x03c8为显卡画板端口号
	for(i = start;i <= end;i++){ //循环，从头到尾遍历画板
		io_out8(0x03c9,rgb[0]/4); //向0x03c9写入RGB颜色编码
		io_out8(0x03c9,rgb[1]/4);
		io_out8(0x03c9,rgb[2]/4);
		rgb +=3; //数组向后偏移3位（3个元素一组形成RGB编码）
	}
	io_store_eflags(eflags); //保存EGLAGS标志寄存器信息
}

/*桌面初始化*/
void initDesk(char *vram,short scrx,short scry){
	BOX deskWindows[] = {
		{0,0,scrx - 1,scry - 15,COL8_004276,"DeskFore"}, //桌面背景色
		{0,scry - 15,scrx - 1,scry - 1,COL8_005B9E,"TaskBar"}, //任务栏
		{0,scry - 1,15,scry - 15,COL8_E1E1E1,"StartDrw"}, //开始菜单渲染
		{0,scry - 15,15,scry - 2,COL8_0078D7,"StartBtn"}, //开始菜单
		{scrx-4,scry-15,scrx-4,scry,COL8_E1E1E1,"DeLine"}, //任务栏分割线
		{5,5,25,25,COL8_E1E1E1,"IconDrw"}, //桌面图标渲染
		{6,6,24,24,COL8_0078D7,"IconInit"}, //桌面图标
		{-1,0,0,0,0,"EndFlag"} //结束标记
	};
	boxDrawx(vram,scrx,deskWindows);
}

/*像素点渲染*/
void pointDraw8(char *vram,short scrx,PIX_POINT point){
	vram[point.py * scrx + point.px] = point.vcolor;
}

/*色块渲染*/
void boxDraw8(unsigned char *vram,int scrx,int bx0,int by0,int bx1,int by1,unsigned char vcolor){
//void boxDraw8(char *vram,short scrx,int bx0,int by0,int bx1,int by1,unsigned int vcolor){
	int vx,vy; //用于遍历的像素坐标
	for(vy = by0;vy <= by1;vy++){
		for(vx = bx0;vx <= bx1;vx++){
			vram[vy * scrx + vx] = vcolor; //根据行列计算出地址偏移量，将vclolor写入以varm为基地址、以y*width+x为偏移量的地址中，即显存的相应位置
		}
	}
}

/*色块批量渲染*/
void boxDrawx(char *vram,short scrx,BOX *box){ //矩形区域绘制函数
	int vx,vy; //用于遍历的像素坐标
	while(box->bx0 != -1){
		for(vy = box->by0;vy <= box->by1;vy++){
			for(vx = box->bx0;vx <= box->bx1;vx++){
				vram[vy * scrx + vx] = box->vcolor; //根据行列计算出地址偏移量，将vclolor写入以varm为基地址、以y*width+x为偏移量的地址中，即显存的相应位置
			}
		}
		box++;
	}
	return;
}

/*图形绘制函数*/
void pictureDraw8(char *varm,short scrx,PICTURE picture){
	int cx,cy; //行列坐标
	for(cy = 0;cy <  CURSOR_PIX;cy++){
		for(cx = 0;cx < CURSOR_PIX;cx++){
			varm[( picture.py + cy) * scrx + ( picture.px + cx)] =  picture.base[cy * CURSOR_PIX + cx];
		}
	}
}


/*字体渲染*/
void fontDraw8(char *vram,int scrx, int fx, int fy, char vcolor, char *font){
	COUNT i = 0; //计数器
	char *p,fontCell; //像素地址指针和单个字体像素数据
	for (i = 0;i < FONT_SIZE;i++){ //逐行扫描写入显存,FONT_SIZE为字符位数常量，见sysdef.h
		p = vram + (fy + i) * scrx + fx;
		fontCell = font[i];
		if ((fontCell & 0x80) != 0) { p[0] = vcolor; } //将像素数据与当前位作与运算，非0即写入当前位
		if ((fontCell & 0x40) != 0) { p[1] = vcolor; }
		if ((fontCell & 0x20) != 0) { p[2] = vcolor; }
		if ((fontCell & 0x10) != 0) { p[3] = vcolor; }
		if ((fontCell & 0x08) != 0) { p[4] = vcolor; }
		if ((fontCell & 0x04) != 0) { p[5] = vcolor; }
		if ((fontCell & 0x02) != 0) { p[6] = vcolor; }
		if ((fontCell & 0x01) != 0) { p[7] = vcolor; }
	}
}

/* void fontDraw8(char *vram,int scrx, int fx, int fy, char vcolor, char *font){
	int i;
	char *p, d ;
	for (i = 0; i < 16; i++) {
		p = vram + (fy + i) * scrx + fx;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = vcolor; }
		if ((d & 0x40) != 0) { p[1] = vcolor; }
		if ((d & 0x20) != 0) { p[2] = vcolor; }
		if ((d & 0x10) != 0) { p[3] = vcolor; }
		if ((d & 0x08) != 0) { p[4] = vcolor; }
		if ((d & 0x04) != 0) { p[5] = vcolor; }
		if ((d & 0x02) != 0) { p[6] = vcolor; }
		if ((d & 0x01) != 0) { p[7] = vcolor; }
	}
} */


/*字符串批量渲染*/
/* void wordsDraw8(char *vram,int scrx,int fx,int fy,char vcolor,unsigned char *words){ //批量写字符
	//COUNT i;
	extern char fonbase[4096]; //引入字库
	TASK *task = taskNow(); //获取当前任务
	char *fonts = (char *) *((int *) 0x0fe8); //日文字库地址
	if(task->lang_mode == 0){
		while(*words != 0x00){ //字符串皆以0x00结尾，故可以此判断是否写完
			fontDraw8(vram,scrx,fx,fy,vcolor,fonbase + *words * 16);
			//fx = fx + 8;
			fx += 8; //横坐标偏移8位（字符为16*8)
			words++; //字符串指针后移
		}		
	}
	// else{
	//	while(*words != 0x00){
	//		fontDraw8(vram,scrx,fx,fy,vcolor,fonts + *words * 16);
	//		fx += 8; //横坐标偏移8位
	//		words++; //字符串指针后移
	//	}		
	//}
	if (task->lang_mode == 1) {
		while(*words != 0x00){
			fontDraw8(vram,scrx,fx,fy,vcolor,fonts + *words * 16);
			fx += 8; //横坐标偏移8位
			words++; //字符串指针后移
		}
	}
	if (task->lang_mode == 2) {
		//for (; *s != 0x00; s++) {
		while(*words != 0x00){
			if (task->lang_byte == 0) {
				if (0xa1 <= *words && *words <= 0xfe) {
					task->lang_byte = *words;
				}
				else {
					fontDraw8(vram, xsize, fx, fy, c, fonts + *words * 16);
				}
			}
			else{
				k = task->lang_byte - 0xa1;
				t = *words - 0xa1;
				task->lang_byte = 0;
				font = fonts + 256 * 16 + (k * 94 + t) * 32;
				fontDraw8(vram, xsize, fx - 8, fy, c, font );
				fontDraw8(vram, xsize, fx , fy, c, font + 16);
			}
			fx += 8;
		}
	}	
} */

void wordsDraw8(char *vram,int scrx,int fx,int fy,char vcolor,unsigned char *words){ //批量写字符
//void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s){
	extern char fonbase[4096]; //引入字库
	struct TASK *task = taskNow(); //获取当前任务
	char *fonts = (char *) *((int *) 0x0fe8), *font; //日文字库地址
	int k, t; //k，存放区号；t，存放点号。存放减1后的值（方便数组运用）

	if (task->lang_mode == 0) {
		for (; *words != 0x00; words++) {
			fontDraw8(vram, scrx, fx, fy, vcolor, fonbase + *words * 16);
			fx += 8;
		}
	}
	if (task->lang_mode == 1) {
		for (; *words != 0x00; words++) {
			if (task->lang_byte == 0) {
				if ((0x81 <= *words && *words <= 0x9f) || (0xe0 <= *words && *words <= 0xfc)) {
					task->lang_byte = *words;
				}
				else {
					fontDraw8(vram, scrx, fx, fy, vcolor, fonts + *words * 16);
				}
			}
			else {
				if (0x81 <= task->lang_byte && task->lang_byte <= 0x9f) {
					k = (task->lang_byte - 0x81) * 2;
				} else {
					k = (task->lang_byte - 0xe0) * 2 + 62;
				}
				if (0x40 <= *words && *words <= 0x7e) {
					t = *words - 0x40;
				} else if (0x80 <= *words && *words <= 0x9e) {
					t = *words - 0x80 + 63;
				} else {
					t = *words - 0x9f;
					k++;
				}
				task->lang_byte = 0;
				font = fonts + 256 * 16 + (k * 94 + t) * 32;
				fontDraw8(vram, scrx, fx - 8, fy, vcolor, font     );	//左半部分
				fontDraw8(vram, scrx, fx    , fy, vcolor, font + 16);	//右半部分
			}
			fx += 8;
		}
	}
	if (task->lang_mode == 2) {
		for (; *words != 0x00; words++) {
			if (task->lang_byte == 0) {
				if (0x81 <= *words && *words <= 0xfe) {
					task->lang_byte = *words;
				} else {
					fontDraw8(vram, scrx, fx, fy, vcolor, fonts + *words * 16);
				}
			} else {
				k = task->lang_byte - 0xa1;
				t = *words - 0xa1;
				task->lang_byte = 0;
				font = fonts + 256 * 16 + (k * 94 + t) * 32;
				fontDraw8(vram, scrx, fx - 8, fy, vcolor, font     );	//左半部分
				fontDraw8(vram, scrx, fx    , fy, vcolor, font + 16);	//右半部分
			}
			fx += 8;
		}
	}
	if (task->lang_mode == 3) {
		for (; *words != 0x00; words++) {
			if (task->lang_byte == 0) {
				if (0xa1 <= *words && *words <= 0xfe) {
					task->lang_byte = *words;
				} else {
					fontDraw8(vram, scrx, fx, fy, vcolor, fonbase + *words * 16);
					
				}
			}
			else{
				k = task->lang_byte - 0xa1;
				t = *words - 0xa1;
				task->lang_byte = 0;
				font = fonts + (k * 94 + t) * 32;
				fontDraw32(vram,scrx,fx-8,fy,vcolor,font,font+16);
			}
			fx += 8;
		}
	}
}

/* void wordsDraw8(char *vram,int scrx,int fx,int fy,char vcolor,unsigned char *words){ //批量写字符
//void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s){
	extern char fonbase[4096];
	struct TASK *task = taskNow();
	char *fonts = (char *) *((int *) 0x0fe8);

	if (task->lang_mode == 0) {
		for (; *words != 0x00; words++) {
			fontDraw8(vram, scrx, fx, fy, vcolor, fonbase + *words * 16);
			fx += 8;
		}
	if (task->lang_mode == 1) {
		for (; *words != 0x00; words++) {
	}
			fontDraw8(vram, scrx, fx, fy, vcolor, fonts + *words * 16);
			fx += 8;
		}
	}
} */

void fontDraw32(char *vram,int scrx,int fx,int fy,char vcolor,char *font1,char *font2){ //汉字渲染引擎
    int i,k,j,f;
	char *p;
    //char *p,d;
    j = 0;
    p = vram+(fy + j) * scrx + fx;
    j++;
    for(i = 0;i < 16;i++){  //上半部分
        for(k=0;k<8;k++){
            if(font1[i] & (0x80 >> k)){
                p[k + (i % 2) * 8] = vcolor;
            }
        }
        for(k = 0;k < 8/2;k++){
            f = p[k + (i % 2) * 8];
            p[k + (i % 2) * 8] = p[8 - 1- k + (i % 2) * 8];
            p[8 - 1 - k + (i % 2) * 8] = f;
        }
        if(i % 2){
            p = vram + (fy + j) * scrx + fx;
            j++;
        }
    }
    
    for(i = 0;i < 16;i++){ //下半部分
        for(k = 0;k < 8;k++){
            if(font2[i] & (0x80 >> k)){
                p[k + (i % 2) * 8] = vcolor;
            }
        }
        for(k = 0;k < 8 / 2;k++){
            f = p[k + (i % 2) * 8];
            p[k + (i % 2) * 8] = p[8 - 1 - k + (i % 2) * 8];
            p[8-1-k+(i%2)*8]=f;
        }
        if(i % 2){
            p = vram + (fy + j) * scrx + fx;
            j++;
        }
    }
}

/*窗口绘制函数*/
void windowDraw8(char *vram,short scrx,WINDOW window,int focus,int area){
	BOX *box = NormalBox;
	//char version[20];
	//COUNT i = 0;
 	switch (window.type)
	{
		case 1: box = ControlBox;break;
		case 2: box = NormalBox;break;
		case 3: box = InformationBox;break;
		case 4: box = textBox;break;
		default: box = NormalBox;break;
	}
	if(window.type == 3){
		//InformationBox[1][4] = COL8_0078D7;
		box = InformationBox;
	}
	box[0].bx0 = window.X;
	box[0].by0 = window.Y;	
	box[0].bx1 = window.X + window.width;
	box[0].by1 = window.Y + window.height;

	box[1].bx0 = box[0].bx0;
	box[1].by0 = box[0].by0;
	box[1].bx1 = box[0].bx1;	
	box[1].by1 = box[1].by0 + WINDOW_CAPTION_HEIGHT;
	

	
	BOX windowLine[] = {
    	{box[0].bx0,box[0].by0,box[0].bx1,box[0].by0,COL8_D9D9D9,"topLine"}, //顶部边线
    	{box[0].bx0,box[0].by1,box[0].bx1,box[0].by1,COL8_D9D9D9,"btomLine"}, //底部边线（最后一个参数不能超过8字节，故缩写）
    	{box[0].bx0,box[0].by0,box[0].bx0,box[0].by1,COL8_D9D9D9,"lifeLine"}, //左侧边线
    	{box[0].bx1,box[0].by0,box[0].bx1,box[0].by1,COL8_D9D9D9,"rigtLine"}, //右侧边线
    	{-1,0,0,0,0,"EndFlag"} //结束标记
	};
	/* BOX windowDiviLine[] = {
		{box[0].bx0,box[1].by1,box[0].bx1,box[1].by1,COL8_D9D9D9,"rigtLine"}, //分割线
    	{-1,0,0,0,0,"EndFlag"} //结束标记
	}; */
	BOX closeButton[] = {
		{box[0].bx1 - 17,box[1].by0 + 1,box[0].bx1 - 1,box[1].by1 - 1,COL8_E81123,"rigtLine"}, //分割线
    	{-1,0,0,0,0,"EndFlag"} //结束标记
	};	
	if(focus == 1){
		box[1].vcolor = COL8_0078D7;
		windowLine[0].vcolor = COL8_0078D7;
		windowLine[1].vcolor = COL8_0078D7;
		windowLine[2].vcolor = COL8_0078D7;
		windowLine[3].vcolor = COL8_0078D7;
		closeButton[0].vcolor = COL8_E81123;
	}
	else{
		box[1].vcolor = COL8_E1E1E1;
		windowLine[0].vcolor = COL8_D9D9D9;
		windowLine[1].vcolor = COL8_D9D9D9;
		windowLine[2].vcolor = COL8_D9D9D9;
		windowLine[3].vcolor = COL8_D9D9D9;	
		closeButton[0].vcolor = COL8_E1E1E1;
	}
	
	if(area == REFRESH_CAPTION){
		//box[0].bx1 = box[0].bx0;
		//box[0].by1 = box[0].by1;	
		BOX boxCaption[] = {
			{box[1].bx0,box[1].by0,box[1].bx1,box[1].by1,box[1].vcolor,"Caption"},
			{-1,0,0,0,0,"EndFlag"} //结束标记
		}; 
		boxDrawx(vram,scrx,boxCaption);
	}
	else{
		boxDrawx(vram,scrx,box);
	}
	wordsDraw8(vram,scrx,box[1].bx0 + 2,box[1].by0 + 1,window.foreColor,window.caption);
	boxDrawx(vram,scrx,closeButton);
	wordsDraw8(vram,scrx,closeButton[0].bx0 + 5 ,closeButton[0].by0,COL8_FFFFFE,"x");	
	//boxDrawx(vram,scrx,windowDiviLine);
	boxDrawx(vram,scrx,windowLine);
	/* if(window.type == 1){
		sprintf(version," NNRJ NNOS [version %s]",NNOS_VERSION);  //输出版本信息
		//wordsDraw8(vram,scrx,box[0].bx0 + 8,box[0].by0 + 8 + WINDOW_CAPTION_HEIGHT,COL8_38CE2F," NNRJ NNOS [version 0.20c]");
		wordsDraw8(vram,scrx,box[0].bx0 + 8,box[0].by0 + 8 + WINDOW_CAPTION_HEIGHT,COL8_38CE2F,version);
		//wordsDraw8(vram,scrx,box[0].bx0 + 8,box[0].by0 + 8 + WINDOW_CAPTION_HEIGHT,COL8_38CE2F,strcat(" NNRJ NNOS "));
		wordsDraw8(vram,scrx,box[0].bx0 + 8,box[0].by0 + 24 + WINDOW_CAPTION_HEIGHT,COL8_38CE2F,"(c)2019 NNRJ Corporation. All Rights Reserved.");
	} */
}

/*创建自动窗口*/
void createWindow(unsigned char *coverBuffer, int width, int height, char *caption,int type,int focus){
	WINDOW autoInfoWindow = {type,0,0,width - 2,height - 2,caption,COL8_000000,COL8_FFFFFE,1};
	windowDraw8(coverBuffer,width,autoInfoWindow,focus,REFRESH_ALL);
}

/*刷新窗口标题*/
void refreshWindowCaption(unsigned char *coverBuffer, int width, int height, char *caption,int type,int focus){
	WINDOW autoInfoWindow = {type,0,0,width - 2,height - 2,caption,COL8_000000,COL8_FFFFFE,1};
	windowDraw8(coverBuffer,width,autoInfoWindow,focus,REFRESH_CAPTION);
}

/*标签绘制*/
/* void labelDraw(SHEET *cover,int x,int y,int foreColor,int backColor,char *s,int len){
	boxDraw8(cover->buf,cover->bxsize,x,y,x + len * 8 - 1,y + 15,backColor);
	wordsDraw8(cover->buf,cover->bxsize,x,y,foreColor,s);
	sheet_refresh(cover,x,y,x + len * 8,y + 16);
} */
/* void labelDraw(SHEET *cover,int x,int y,int foreColor,int backColor,char *s,int len){
	//TASK *task = taskNow();
	boxDraw8(cover->buf,cover->bxsize,x,y,x + len * 8 - 1,y + 15,backColor);
	wordsDraw8(cover->buf,cover->bxsize,x,y,foreColor,s);
	sheet_refresh(cover,x,y,x + len * 8,y + 16);
} */

void labelDraw(SHEET *cover,int x,int y,int foreColor,int backColor,char *s,int len){
//void labelDraw(struct SHEET *cover, int x, int y, int foreColor, int backColor, char *s, int len){
	TASK *task = taskNow();
	boxDraw8(cover->buf, cover->bxsize, x, y, x + len * 8 - 1, y + 15, backColor);
	if (task->lang_mode != 0 && task->lang_byte != 0) {
		//putfonts8_asc(cover->buf, cover->bxsize, x, y, foreColor, s);
		wordsDraw8(cover->buf,cover->bxsize,x,y,foreColor,s);
		sheet_refresh(cover, x - 8, y, x + len * 8, y + 16);
	} else {
		//putfonts8_asc(cover->buf, cover->bxsize, x, y, foreColor, s);
		//sheet_refresh(cover, x, y, x + len * 8, y + 16);
		wordsDraw8(cover->buf,cover->bxsize,x,y,foreColor,s);
		sheet_refresh(cover,x,y,x + len * 8,y + 16);
	}
}

/*鼠标指针图像点阵解析引擎*/
void initMouseCursor8(char *cusorGraph,char curBackColor){
	int cx,cy; //行列坐标
	for(cx = 0;cx < CURSOR_WIDTH;cx++){ 		//遍历鼠标指针二维点阵，根据点阵不同字符，赋值不同颜色
		for(cy = 0;cy < CURSOR_HEIGHT;cy++){
			switch (CUSOR_GRAPH[cx][cy]){
				case '*' : {
					cusorGraph[cx * 16 + cy] = COL8_000000;break; //字符'*'处指针像素颜色为纯黑
				}	
				case '1' : {
					cusorGraph[cx * 16 + cy] = COL8_FFFFFF;break; //字符'O'处指针像素颜色为纯白
				}
				//case '.':mouse[cx * 16 + cy] = curBackColor;break; //字符'.'处指针像素颜色为背景色
				default : {
					cusorGraph[cx * 16 + cy] = curBackColor;break; //其他情况指针像素颜色为背景色
				}
			}
		}
	}
}

/*创建文本框*/
void makeTextBox8(SHEET *sht,int x0,int y0,int width,int htight,int c){
	int x1 = x0 + width, y1 = y0 + htight;
	//boxDraw8(char *vram,short scrx,int bx0,int by0,int bx1,int by1,unsigned int vcolor)
	boxDraw8(sht->buf, sht->bxsize, x0 - 2, y0 - 3, x1 + 1, y0 - 3, COL8_848484);
	boxDraw8(sht->buf, sht->bxsize, x0 - 3, y0 - 3, x0 - 3, y1 + 1, COL8_848484);
	boxDraw8(sht->buf, sht->bxsize, x0 - 3, y1 + 2, x1 + 1, y1 + 2, COL8_F0F0F0);
	boxDraw8(sht->buf, sht->bxsize, x1 + 2, y0 - 3, x1 + 2, y1 + 2, COL8_F0F0F0);
	boxDraw8(sht->buf, sht->bxsize, x0 - 1, y0 - 2, x1 + 0, y0 - 2, COL8_000000);
	boxDraw8(sht->buf, sht->bxsize, x0 - 2, y0 - 2, x0 - 2, y1 + 0, COL8_000000);
	boxDraw8(sht->buf, sht->bxsize, x0 - 2, y1 + 1, x1 + 0, y1 + 1, COL8_C6C6C6);
	boxDraw8(sht->buf, sht->bxsize, x1 + 1, y0 - 2, x1 + 1, y1 + 1, COL8_C6C6C6);
	boxDraw8(sht->buf, sht->bxsize,           x0 - 1, y0 - 1, x1 + 0, y1 + 0, c);
}

/*窗口直线绘制*/
void syslinewin(SHEET *sht,int x0,int y0,int x1,int y1,int vcolor){ //窗口直线绘制
	int i,x,y,len,dx,dy;
	dx = x1 - x0;dy = y1 - y0;
	x = x0 << 10;y = y0 << 10;
	
	if(dx < 0){
		dx =- dx;
	}
	if(dy < 0){
		dy =- dy;
	}
	if(dx >= dy){
		len = dx + 1;
		if(x0 > x1){
			dx =- 1024;
		}
		else{
			dx = 1024;
		}
		if(y0 <= y1){
			dy =((y1 - y0 + 1) << 10) / len;
		}
		else{
			dy=((y1 - y0 - 1) << 10) / len;
		}
	}else{
		len = dy + 1;
		if(y0 > y1){
			dy =-1024;
		}
		else{
			dy = 1024;
		}
		if(x0 <= x1){
			dx=((x1 - x0 + 1) << 10) / len;
		}
		else{
			dx=((x1 - x0 - 1) << 10) / len;
		}
	}
	for(i = 0;i < len;i++){
		sht->buf[(y >> 10) * sht->bxsize+(x >> 10)] = vcolor;
		x += dx;
		y += dy;
	}
}

/*测试函数*/


//=====已废弃的函数=========================================================================================================
/* void syslinewin(SHEET *sht,int x0,int y0,int x1,int y1,int vcolor){
	int i,x,y,len,dx,dy;
	dx = x1 - x0;
	dy = y1 - y0;
	x = x0 << 10;
	y = y0 << 10;
	if(dx < 0){
		dx = -dx;
	}
	if(dy < 0){
		dy = -dy;
	}
	//if(dx >= dy){
		
	//}
	
} */

/* void make_window8(unsigned char *buf, int xsize, int ysize, char *title){
	int x, y;
	char c;
 	BOX testWindow[] = {
		{0,0,xsize - 1,0,COL8_C6C6C6,"001"}, 
		{1,1,xsize - 2,1,COL8_FFFFFF,"002"}, 
		{0,0,0,ysize - 1,COL8_C6C6C6,"003"}, 
		{1,1,1,ysize - 2,COL8_FFFFFF,"004"}, 
		{xsize - 2, 1,xsize - 2,ysize - 2,COL8_848484,"005"}, 
		{xsize - 1, 0,xsize - 1,ysize - 1,COL8_000000,"006"}, 
		{2,2,xsize - 3,ysize - 3,COL8_C6C6C6,"007"},
		{3,3,xsize - 4,20,COL8_000084,"008"},
		{1,ysize - 2,xsize - 2,ysize - 2,COL8_848484,"009"},
		{0,ysize - 1,xsize - 1,ysize - 1,COL8_000000,"010"},
		{-1,0,0,0,"EndFlag"} //结束标记
	};
	boxDrawx(buf,xsize,testWindow);
	//WINDOW testInfoWindow = {3,xsize - 2,ysize - 2,0,0,"Welcome",COL8_000000,COL8_FFFFFE,1,1};
	//windowDraw8(buf,xsize,testInfoWindow);
	wordsDraw8(buf,xsize,24,4,COL8_FFFFFF,title); 	
 	//for (y = 0; y < 14; y++) {
	//	for (x = 0; x < 16; x++) {
	//		c = CLOSE_BUTTON[y][x];
	//		if (c == '@') {
	//			c = COL8_000000;
	//		} else if (c == '$') {
	//			c = COL8_848484;
	//		} else if (c == 'Q') {
	//			c = COL8_C6C6C6;
	//		} else {
	//			c = COL8_FFFFFF;
	//		}
	//		buf[(5 + y) * xsize + (xsize - 21 + x)] = c;
	//	}
	//}
} */

/*鼠标指针定位绘制函数*/
/* void mouseCursorDraw8(char *vram,short scrx){
	
}
 */
/*图形绘制函数*/
/* void pictureDraw8(char *varm,short scrx,PICTURE cursorPic){
	int cx,cy; //行列坐标
	for(cy = 0;cy < cursorPic.pix_size;cy++){
		for(cx = 0;cx <cursorPic.pix_size;cx++){
			varm[(cursorPic.pic_Y + cy) * scrx + (cursorPic.pic_X + cx)] = cursorPic.pic_base[cy * cursorPic.pix_size + cx];
		}
	}
} */

/*像素线渲染函数*/
/* void lineDraw8(char *vram,Line *line){
	int vx,vy;
	Box *box;
	while(line->lx != -1){
		box->bx
		//for(vx = line->lx;vx <= line->width;vx++)
	}
} */

/*屏幕信息初始化函数*/
/* void initScreen(SCREEN *screen,BOOT_INFO *bootInfo){
	screen->scrx = bootInfo->screen_width;
	screen->scry = bootInfo->screen_height;
	screen->centerX = (screen->scrx - 16) / 2;
	screen->centerY = (screen->scry - 16) / 2;
} */


