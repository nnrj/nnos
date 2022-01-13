/**
 * ===================NNOS-graphics===========================
 *【文件相关函数】【核心层】【文件管理系统】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/
#include "./lib/nnos.h"	//系统函数声明
int consoleX = 0; int consoleY = 0;

/*解码和装载FAT*/
void readFAT(int *fat,unsigned char *img){ //FAT解码
	COUNT i = 0;
	COUNT j = 0;
	for(i = 0;i < 2880;i += 2){ //微软FAT解码算法
		fat[i + 0] = (img[j + 0] | img[j + 1] << 8) &0xfff; 
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j +=3;
	}
}

/*加载文件*/
void loadFile(int cluster,int size,char *file,int *fat,char *img){
	COUNT i = 0;
	while(1){
		if(size <= 512){ //每簇大小为一个扇区，一个扇区大小为512字节
			for(i = 0;i < size;i++){
				file[i] = img[cluster * 512 + i];
			}
			break;
		}
		for(i = 0;i < 512;i++){
			file[i] = img[cluster * 512 + i];
		}
		size -= 512;
		file += 512;
		cluster = fat[cluster];
	}
}

/* void loadFile(int cluster,int size,char *file,int *fat,char *img){
//void file_loadfile(int cluster, int size, char *file, int *fat, char *img){
	int i;
	while(1){
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				file[i] = img[cluster * 512 + i];
			}
			break;
		}
		for (i = 0; i < 512; i++) {
			file[i] = img[cluster * 512 + i];
		}
		size -= 512;
		file += 512;
		cluster = fat[cluster];
	}
} */

/* char *loadFile2(int cluster, int *psize, int *fat){
	int size = *psize,size2;
	MEMERY_LIST *memeryList = (MEMERY_LIST *)MEMERY_ADDR;
	char *buffer, *buffer2;
	buffer = (char *)memeryAlloc4k(memeryList,size);
	file_loadfile(cluster, size, buffer, fat, (char *) (DISK_ADR + 0x003e00));
	if (size >= 17){
		size2 = tek_getsize(buffer);
		if (size2 > 0){
			buffer2 = (char *) memeryAlloc4k(memeryList,size2);
			tek_decomp(buffer, buffer2, size2);
			memeryFree4k(memeryList, (int) buffer,size);
			buffer = buffer2;
			*psize = size2;
		}
	}
	return buffer;
} */


/*查找文件*/
//FILE_INFO *searchFile(char *fileName,CONSOLE *console){
FILE_INFO *searchFile(char *fileName){
	//int i,j;
	COUNT i = 0;
	COUNT j = 0;
	char temp[5];
	FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	i = 0;j = 0; //初始化游标
	int flag = 0;
	convertToUppercase(fileName); //转为大写
	for(i = 0;i < FILE_INFO_MAX;i++){
		if(fileInfo[i].name[0] == 0x00){ //无文件信息
			break;
		}
		if((fileInfo[i].type & 0x18) == 0){ //权限正常
			for(j = 0;j < FILE_FULL_NAME_SIZE;j++){
				if(fileInfo[i].name[j] == fileName[j]){
					flag = 1;
				}
				else{
					flag = 0;
					break;
				}				
			}
			if(flag == 1){	
				return &fileInfo[i];
				//break;
			}
		}
	}
	/* if(flag == 1){	
		return &fileInfo[i];
	}
	else{		
		return 0;
	}	 */	
	return 0;
}

int *deleteFile(char *fileName){
	//int i,j;
	COUNT i = 0;
	COUNT j = 0;
	COUNT t = -1;
	char temp[5];
	FILE_INFO *fileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	i = 0;j = 0; //初始化游标
	int flag = 0;
	convertToUppercase(fileName); //转为大写
	for(i = 0;i < FILE_INFO_MAX;i++){
		if(fileInfo[i].name[0] == 0x00){ //无文件信息
			break;
		}
		if((fileInfo[i].type & 0x18) == 0){ //权限正常
			for(j = 0;j < FILE_FULL_NAME_SIZE;j++){
				if(fileInfo[i].name[j] == fileName[j]){
					flag = 1;
				}
				else{
					flag = 0;
					break;
				}				
			}
			if(flag == 1){	
				////return &fileInfo[i];
				t = i;
				for(j = t;j < FILE_INFO_MAX;j++){
					if(fileInfo[j + 1].name[0] != 0){
						fileInfo[j] = fileInfo[j + 1];
					}
					else{
						fileInfo[j].name[0] = 0;
						break;
					}
					
					
				}
				//fileInfo[t].name[0] = 0;
				return 1;
				//break;
			}
		}
	}
	/* if(flag == 1){	
		return &fileInfo[i];
	}
	else{		
		return 0;
	}	 */	
	return 0;
}

/*未完成的引擎：文件写入引擎【注意，此函数尚未在nnos.h注册】*/
/* int writeFile(FILE_INFO *fileInfo,int size,char *file,int *fat,char *img){
	int cluster;
	FILE_INFO *myFileInfo = (FILE_INFO *)(DISK_ADR + 0X002600);
	while(myFileInfo[i].name[0] != 0x00){
		i++;
		if(i >= FILE_INFO_MAX){
			return 0;
		}
	}
	myFileInfo[i] = fileInfo;
	myFileInfo[i].cluster = myFileInfo[i - 1].cluster + 1;
	putFile(myFileInfo[i].cluster,size,file,fat,img);
	
} */

/*未完成的函数：数据写入函数【注意，此函数尚未在nnos.h注册】*/
/* void putFile(int cluster,int size,char *file,int *fat,char *img){
	COUNT i = 0;
	while(1){
		if(size <= 512){ //每簇大小为一个扇区，一个扇区大小为512字节
			for(i = 0;i < size;i++){
				img[cluster * 512 + i] = file[i];
			}
			break;
		}
		for(i = 0;i < 512;i++){
			img[cluster * 512 + i] = file[i];
		}
		size += 512;
		file -= 512;
		fat[cluster] = cluster;
	}
} */
/* FILE_INFO *file_search(char *name,FILE_INFO *finfo,int max){
	int i, j;
	char s[12];
	for(j = 0; j < 11; j++){
		s[j] = ' ';
	}
	j = 0;
	for(i = 0; name[i] != 0; i++){
		if (j >= 11) { return 0;}
		if (name[i] == '.' && j <= 8){
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z'){
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for(i = 0; i < max; ) {
		if (finfo->name[0] == 0x00){
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i;
		}
next:
		i++;
	}
	return 0;
} */

/* FILE_INFO *file_search(char *name,int max){
	int i, j;
	char s[12];
	FILE_INFO *finfo = (FILE_INFO *)(DISK_ADR + 0x002600);
	for(j = 0; j < 11; j++){
		s[j] = ' ';
	}
	j = 0;
	for(i = 0; name[i] != 0; i++){
		if (j >= 11) { return 0;}
		if (name[i] == '.' && j <= 8){
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z'){
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for(i = 0; i < max; ) {
		if (finfo->name[0] == 0x00){
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i;
		}
next:
		i++;
	}
	return 0;
} */

/* FILE_INFO *file_search(char *name){
	int i, j;
	char s[12];
	FILE_INFO *finfo = (FILE_INFO *)(DISK_ADR + 0x002600);
	for(j = 0; j < 11; j++){
		s[j] = ' ';
	}
	j = 0;
	for(i = 0; name[i] != 0; i++){
		if (j >= 11) { return 0;}
		if (name[i] == '.' && j <= 8){
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z'){
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for(i = 0; i < FILE_INFO_MAX; ) {
		if (finfo->name[0] == 0x00){
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return finfo + i;
		}
next:
		i++;
	}
	return 0;
} */

FILE_INFO *file_search2(char *name, struct FILE_INFO *fileInfo, int max){
	int i, j;
	char s[12];
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) { return 0; }
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for (i = 0; i < max; ) {
		if (fileInfo->name[0] == 0x00) {
			break;
		}
		if ((fileInfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (fileInfo[i].name[j] != s[j]) {
					goto next;
				}
			}
			return fileInfo + i; 
		}
next:
		i++;
	}
	return 0; 
}


