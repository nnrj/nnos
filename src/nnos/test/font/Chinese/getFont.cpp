#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <pshpack2.h>
#include <iostream>
  
#define size 32
#define sum (size*size/8)

using namespace std; 
  
void convert(unsigned char qh,unsigned char wh);
void Snapshot( BYTE * pData, int width, int height,  char * filename );
  
typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
  
typedef struct {
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BMPFILEHEADER_T;
  
struct BMPFILEHEADER_S{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
};
  
typedef struct{
    DWORD      biSize;
    LONG       biWidth;
    LONG       biHeight;
    WORD       biPlanes;
    WORD       biBitCount;
    DWORD      biCompression;
    DWORD      biSizeImage;
    LONG       biXPelsPerMeter;
    LONG       biYPelsPerMeter;
    DWORD      biClrUsed;
    DWORD      biClrImportant;
} BMPINFOHEADER_T;
  
int main()
{
    //提取某段汉字
    for (unsigned char i=16;i<=16;i++)
    {
        for (unsigned char j=1;j<=3;j++)
        {
            convert(i,j);
        }
    }
  
    //提取某个汉字
    unsigned char incode[3] = "我";
    unsigned char qh = incode[0] - 0xa0; //获得区码  
    unsigned char wh = incode[1] - 0xa0; //获得位码
    convert(qh,wh);
  
    return 0;
}
  
  
void convert(unsigned char qh,unsigned char wh) 
{    
    unsigned long offset = 0;
  
    unsigned char mat[sum][size/8] = {0} ; //mat[sum][2]
  
    struct {
        BYTE b;
        BYTE g;
        BYTE r;
    } pRGB[size][size];  // 定义位图数据
    memset( pRGB, 0, sizeof(pRGB) ); // 设置背景为黑色
  
    char  hz[64];
    FILE* HZK = NULL;
  
    sprintf(hz , "HZK%d" , size);
  
    HZK = fopen(hz , "rb");
    offset = (94*(qh-1)+(wh-1))*sum; //得到偏移位置
  
    if( HZK == NULL)  
        //fail to open file
    {  
        printf("Can't Open hzk%d\n" , size);  
        getchar(); 
        return ; 
    }
    fseek(HZK, offset, SEEK_SET); 
    fread(mat, sum , 1, HZK);
    fclose(HZK); 
    int col=0;
    int row=0;
    //显示 
    for(int i=0; i<size; i++) //16
    { 
        for(int j=0; j<size/8; j++) //2
        { 
            for(int k=0; k<8 ; k++) //8bits , 1 byte
            { 
                if( mat[i][j] & (0x80>>k))
                {//测试为1的位则显示 
                    //汉字颜色
                    //printf("1");
                    pRGB[size-1-row][col-size*row].r = 0xff;//bmp中的行坐标倒序
                    pRGB[size-1-row][col-size*row].g = 0xff;
                    pRGB[size-1-row][col-size*row].b = 0xff;
                } 
                else 
                {   //背景色
                    //printf("0");
                    pRGB[size-1-row][col-size*row].r = 0x00;//bmp中的行坐标倒序
                    pRGB[size-1-row][col-size*row].g = 0x00;
                    pRGB[size-1-row][col-size*row].b = 0x00;
                } 
                col++;
            }//for
        }//for
        //printf("\n"); 
        row++;
    }//for
    char filename[9];
    sprintf(filename,"%2d%2d.bmp",qh,wh);
    if (wh<10)
        filename[2]='0';
    if (qh<10)
        filename[0]='0';
    Snapshot( ( BYTE*)pRGB, size, size, filename);
}
  
  
  
void Snapshot( BYTE * pData, int width, int height,  char * filename )
{
    int bmpsize = width*height*3; // 每个像素点3个字节
  
    // 位图第一部分，文件信息
    BMPFILEHEADER_T bfh;
    bfh.bfType = 0x4d42;  //bm
    bfh.bfSize = bmpsize  // data size
        + sizeof( BMPFILEHEADER_T ) // first section size
        + sizeof( BMPINFOHEADER_T ) // second section size
        ;
    bfh.bfReserved1 = 1; // reserved 
    bfh.bfReserved2 = 0; // reserved
    bfh.bfOffBits = bfh.bfSize - bmpsize;
    // 位图第二部分，数据信息
    BMPINFOHEADER_T bih;
    bih.biSize = sizeof(BMPINFOHEADER_T);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = bmpsize;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;       
  
    FILE * fp = fopen(filename,"wb");
    if( !fp ) return;
  
    fwrite( &bfh, 1, sizeof(BMPFILEHEADER_T), fp );
    fwrite( &bih, 1, sizeof(BMPINFOHEADER_T), fp );
    fwrite( pData, 1, bmpsize, fp );
    fclose( fp );
}
