/**
 * 系统结构体类型定义
 **/

//=====全局描述符(GDT)===================================================
typedef struct SEGMENT_DESCRIPTOR{  //以CPU信息为基础的结构体，存放8字节内容
    short LIMIT_LOW;                //低位下限
    short BASE_LOW;                 //低位基址
    char BASE_MID;                  //中位基址
    char LIMIT_HIGH;                //高位上限
    char BASE_HIGH;                 //高位基址
    char ACCESS_PER;                //操作权限
}SEGMENT_DESCRIPTOR;

//=====中断描述符(IDT)==================================================
typedef struct GATE_DESCRIPTOR{     //以CPU信息为基础的结构体，存放8字节内容
    short SELECTOR;                 //段选择子（段号）
    short OFFSET_LOW; 
    short OFFSET_HIGH;
    char ACCESS_PER;
    char DW_COUNT;
}GATE_DESCRIPTOR;

//=====系统硬件描述=====================================================
/*BOOT信息结构体*/           //信息数据来自于syshead.asm
typedef struct BOOT_INFO{   //指针指向相应信息
    char *boot_base;        //boot加载位置
    char *disk_base;        //磁盘缓存首地址
    char *cache_base;       //实模式下磁盘高速缓存首地址
    int *vram_base;         //显存首地址
    char *cyls_max;         //磁盘最大装载柱面号（启动区读硬盘读到何处为止）
    char *leds_status;      //键盘LED指示灯状态
    short *screen_X;        //显示器行分辨率（单位：像素）
    short *screen_Y;        //显示器列分辨率（单位：像素）
    char boot_reserve;      //服务  
    char vram_mode;         //显卡模式（位数，颜色模式）
    int boot_key;           //boot密码锁
    int safe_check;         //安全校验 
}BOOT_INFO;

/*屏幕结构体*/
typedef struct SCREEN{
    short screen_width;
    short screen_height;
}SCREEN;

/*显卡结构体*/
typedef struct VRAM{        //信息数据来自于syshead.asm
    char *vram_base;
    //char *end;
    //char *mode;
    //int vwidth;
    //int vheight;
    //int byteSize;
}VRAM;

/*系统信息结构体*/
typedef struct SYSTEM_INFO{
    struct BOOT_INFO SYS_BOOT_INFO;
    struct SCREEN SYS_SCREEN_INFO;
    struct VRAM SYS_VRAM_INFO;
}SYSTEM_INFO;

//=====绘图结构体描述=====================================================
/*像素点结构体*/
typedef struct PIX_POINT{
    int px;                 //点横坐标
    int py;                 //点纵坐标
    unsigned char vcolor;   //填充色
}PIX_POINT;

/*色块结构体*/
typedef struct BOX{
    int bx0;                //矩形横向起点
    int bx1;                //矩形横向终点
    int by0;                //矩形纵向起点
    int by1;                //矩形纵向终点
    unsigned char vcolor;   //填充色
    char box_name[8];
}BOX;

/*图形结构体*/
typedef struct PICTURE{
    int pic_X;              //横坐标
    int pic_Y;              //纵坐标
    int pic_width;          //图形宽度
    int pic_height;         //图形高度
    int pix_size;           //每行像素数
    char *pic_base;         //图形内存基址
}PICTURE;



//=====已废弃的结构体=====================================================
/* typedef struct FONT{ //字体结构体
    int font_size;  //字体大小
    int font_X; //X坐标
    int font_Y; //Y坐标
    unsigned char vcolor; //填充色
    char *font_char;
    //char font_char;
}FONT; */

/* typedef struct FONT{ //字体结构体
    int font_size;  //字体大小
    int font_X; //X坐标
    int font_Y; //Y坐标
    unsigned char vcolor; //填充色
    char *font_char;
    //char font_char;
    FONT *next; //指针
}FONT; */

/* typedef struct FONT{ //字体结构体
    int font_size;  //字体大小
    int font_X; //X坐标
    int font_Y; //Y坐标
    unsigned char vcolor; //填充色
    char *font_head;
    //char font_char;
    //FONT *next; //指针
}FONT; 
 */
/* typedef struct FONT{ //字体结构体
    int font_size;  //字体大小
    int font_X; //X坐标
    int font_Y; //Y坐标
    unsigned char vcolor; //填充色
    char *font_char;
    //char font_char;
    FONT *next; //指针
}FONT; 

typedef struct WORD{
    FONT word_font;
    char *words;
    //char *word_head;
}WORD; */

/*struct RGBColor{
    int RColor;
    int GColor;
    int BColor;
};*/
//#define FONT_SIZE sizeof(FONT).

/*显卡结构体*/
/* typedef struct VRAM{        //信息数据来自于syshead.asm
    char *vram_base;
    //char *end;
    //char *mode;
    //int vwidth;
    //int vheight;
    //int byteSize;
}VRAM; */

