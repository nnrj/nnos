/**
 * nask源码转nasm源码
 * 
 * */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define N 25000 //设定文件最大字符个数不超过20000，可更改
int main(){
    int i=0,j,k=0,m=0,pos=0,max,len1,len2;
    char s[N],t[N],fileName[200],s1[200],s2[200];
    FILE *fp;
    scanf("%s %s %s",fileName,s1,s2);
    len1=strlen(s1);len2=strlen(s2);
    if ((fp=fopen(fileName,"r"))==NULL){  //设定文件位于当前目录下，可更改为绝对路径
        printf("Open file %s error! Strike any key to exit!",fileName);
        exit(1);
    }
    s[i++]=fgetc(fp);
    while(!feof(fp)){
        s[i++]=fgetc(fp);
    }
    fclose(fp);
    
    max=i-1; //函数feof()最后会读两次，所以必须减1 
    for(i=0;i<max-len1;i++){
        for(j=0;j<len1&&(s[i+j]==s1[j]);j++);
    if(j!=len1)
        continue; //不相等则i加1进行下一次外循环
        else{
            for(k=0;k<i-pos;k++) //pos记录开始复制的位置
                t[m++]=s[pos+k];
            pos=i+len1;
            for(k=0;k<len2;k++) //字符串替换
                t[m++]=s2[k];
        }
    }
    for(k=pos;k<max;k++) //复制剩余字符
        t[m++]=s[k];
    max=m; //max是替换后文件的字符总数
    fp=fopen(fileName,"w");
    for(j=0;j<max;j++){
        //putchar(t[j]); //结果显示到屏幕
        fputc(t[j],fp);  //同时结果写入到当前目录下的新文件001.txt中
    }
    printf("%s_info:getnas is OK!\n",fileName); 
    fclose(fp);
    return 0;
}
