/**
 * ===================NNOS-Devices============================
 *【I/0管理系统】【硬件设备相关】
 * -----------------------------------------------------------
 * ·日期：2019-3-22     作者：年年软件
 * ===========================================================
**/
/*头文件引用*/	
#include "./lib/nnos.h"	//系统函数声明

/*设备缓冲区*/
FIFO_BUFFER keyFIFOBuffer,mouseFIFOBuffer; //键盘、鼠标先进先出缓冲区
FIFO_BUFFER32 *keyFIFOBuffer32,*mouseFIFOBuffer32; //键盘、鼠标32位先进先出缓冲区
int keyDataBase,mouseDataBase;

//=====屏幕处理器================================================================================================
 /*屏幕信息初始化函数*/
void initScreen(SCREEN *screen,BOOT_INFO *bootInfo){
	screen->scrx = bootInfo->screen_width;
	screen->scry = bootInfo->screen_height;
	screen->centerX = (screen->scrx - 16) / 2;
	screen->centerY = (screen->scry - 16) / 2;
	screen->vram = bootInfo->vram_base;
}

//=====键盘处理器================================================================================================
/*等待键盘控制电路准备完毕函数*/
void waitKBCReady(){ //CPU快，键盘慢，需要等待（此代码应改进为响应）
	while(1){
		if((io_in8(PORT_KEYSTA) & KEYSTA_NOTREADY) == 0){ //读取键盘状态，返回值与“键盘未准备好”代码作逐位与，为0则停止等待（确认数据的到倒数第二位为0，从低位开始数第二位）
			break; //停止等待，跳出循环
		}
	}
}

/*键盘控制电路初始化函数*/
void initKeyboardCMD(){ //初始化键盘控制电路,确认是否可往键盘控制电路发送信息
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE); //将键盘控制电路模式指令0x60写入键盘控制电路端口的寄存器
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYDAT,KBC_MODE_MOUSE); //将键盘控制电路模式设置为0x47，鼠标模式
}

/*键盘控制电路初始化函数32位*/
void initKeyboardCMD32(FIFO_BUFFER32 *FIFOBuffer,int dataBase){ //初始化键盘控制电路,确认是否可往键盘控制电路发送信息
	keyFIFOBuffer32 = FIFOBuffer; //通用缓冲区数据存入键盘缓冲区
	keyDataBase = dataBase; //此值或可改常量
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE); //将键盘控制电路模式指令0x60写入键盘控制电路端口的寄存器
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYDAT,KBC_MODE_MOUSE); //将键盘控制电路模式设置为0x47，鼠标模式
}

//=====鼠标处理器================================================================================================
/*鼠标中断数据解码函数*/
int decodeMouse(MOUSE_CURSOR *mouseCursor,unsigned char data){
		if(mouseCursor->phase == 0){
			if(data == 0xfa){
				mouseCursor->phase = 1;
			}
			return 0;
		}
		if(mouseCursor->phase == 1){
			if((data & 0xc8) == 0x08){ //校验第一字节是否正确(判断移动数据是否在0~3范围内，判断点击数据是否在8~F范围内)，不正确则舍弃
				mouseCursor->dataBuffer[0] = data;
				mouseCursor->phase = 2;
			}
			return 0;
		}
		if(mouseCursor->phase == 2){
			mouseCursor->dataBuffer[1] = data;
			mouseCursor->phase = 3;
			return 0;
		}
		if(mouseCursor->phase == 3){
			mouseCursor->dataBuffer[2] = data;
			mouseCursor->phase = 1;
			mouseCursor->mbutton = mouseCursor->dataBuffer[0] & 0x07; //解析鼠标数据，鼠标按键状态位于dataBuffer[0]低3位（00000111）,用与运算取出低3位
			mouseCursor->mx = mouseCursor->dataBuffer[1]; //横向移动数据位于dataBuffer[1]
			mouseCursor->my = mouseCursor->dataBuffer[2]; //纵向移动数据位于dataBuffer[2]
			if((mouseCursor->dataBuffer[0] & 0x10) != 0){ //取出dataBuffer[0]的第1位，判断鼠标移动反应位数
				mouseCursor->mx |= 0xffffff00; //将x第8位及以后置1或保留0
			}
			if((mouseCursor->dataBuffer[0] & 0x20) != 0){
				mouseCursor->my |= 0xffffff00; //将y第8位及以后置1或保留0
			}
			mouseCursor->my = -mouseCursor->my; //对y取反（鼠标Y方向与屏幕Y相反）
			return 1;	
		}
		return -1;
 }
 
 /*激活鼠标函数*/
void enableMouse(MOUSE_CURSOR *mouseCursor){
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYCMD,KEYCMD_MOUSE); //向键盘控制电路发送切换鼠标模式指令(0x64)，发送后下一个的数据会自动发送给鼠标
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYDAT,MOUSECMD_ENABLE); //向键盘控制电路发送激活鼠标代码，激活成功返回值为ACK(0xfa)
	mouseCursor->phase = 0;
}
/*鼠标收到激活指令后，会给CPU发送答复信息，信息代码为0xfa，此时会产生一个鼠标中断*/

 /*激活鼠标函数32位*/
void enableMouse32(FIFO_BUFFER32 *FIFOBuffer,int dataBase,MOUSE_CURSOR *mouseCursor){
	mouseFIFOBuffer32 = FIFOBuffer; //通用缓冲区数据存入鼠标缓冲区
	mouseDataBase = dataBase; //此值或可改常量
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYCMD,KEYCMD_MOUSE); //向键盘控制电路发送切换鼠标模式指令(0x64)，发送后下一个的数据会自动发送给鼠标
	waitKBCReady(); //等待键盘控制电路准备完毕
	io_out8(PORT_KEYDAT,MOUSECMD_ENABLE); //向键盘控制电路发送激活鼠标代码，激活成功返回值为ACK(0xfa)
	mouseCursor->phase = 0;
}
//=====CPU处理器================================================================================================
/*CUP停机等待函数*/
void sysHLT(){
	while(1){
        io_hlt();
    }
}
