/**
 * ===================NNOS-Memery============================
 *【内存管理系统】
 * -----------------------------------------------------------
 * ·日期：2019-3-15      作者：年年软件
 * ===========================================================
**/
/*头文件引用*/	
#include "./lib/nnos.h"	//系统函数声明

#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

/*内存检查函数*/
//486及以上CPU有Cache，需要先禁用Cache才能完成内存检查
unsigned int memeryCheck(unsigned int start,unsigned int end){ 
	char flag486 = 0; //内存检查前的准备工作
	unsigned int eflag,cr0,i;
	eflag = io_load_eflags(); //读取EFLAGS寄存器内容到elag变量
	eflag |= EFLAGS_AC_BITS;   //将eflag的值与AC位存在校验码EFLAGS_AC_BIT做或运算赋值给eflag变量，使得AC位变为1。AC位在第18位。
	io_store_eflags(eflag);   //将eflag的值保存到EFLAGS寄存器
	eflag = io_load_eflags(); //读取EFLAGS寄存器内容到elag变量（386会自动把AC位置回0）。
	if((eflag & EFLAGS_AC_BITS) != 0){ //若相同，AC位未变回0，则为486
		flag486 = 1;
	}
	eflag &= ~EFLAGS_AC_BITS; //恢复AC位数值为0
	io_store_eflags(eflag);  //保存
	
	if(flag486 != 0){
		cr0 = load_cr0();//读取cr0寄存器内容到cr0变量
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}
	
	//i = memeryCheckSub(start,end); //检查内存
	i = memtest_sub(start,end); //检查内存
	
	if(flag486 != 0){
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; //启用Cache
		store_cr0(cr0);
	}
	return i;
}

/* unsigned int memeryCheck(unsigned int start, unsigned int end)
{
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; 
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { 
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT;
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; 
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; 
		store_cr0(cr0);
	}
	return i;
} */

void initMemeryManager(MEMERY_LIST *memeryList){ //初始化内存表
	//memeryList->total_size = memtest(0x00400000, 0xbfffffff);
	memeryList->total_size = memeryCheck(0x00400000,0xbfffffff); //内存总量等于内存检查返回的内存大小
	//memeryFree(memeryList,0x00001000,0x0009e000); //初始空闲内存，开头预留4M给系统
	//memeryFree(memeryList,0x00400000,memeryList->total_size - 0x00400000); //中间预留给显卡(?)
	//memeryFree(memeryList,0x00001000,0x0009e000);
	//memeryFree(memeryList,0x00400000,memeryList->total_size - 0x00400000);
	memeryList->number = 0;	//从0开始计数
	memeryList->max_number = 0;
	memeryList->lost_number = 0;
	memeryList->lost_size = 0;
}

/* void initMemeryManager(MEMERY_LIST *memeryList)
{
	//man->frees = 0;			
	//man->max_number = 0;		
	//man->lostsize = 0;		
	//man->losts = 0;			
	//memeryList->total_size = memtest(0x00400000, 0xbfffffff);
	//memeryList->total_size = memeryCheck(0x00400000,0xbfffffff); //内存总量等于内存检查返回的内存大小
	//memeryFree(memeryList,0x00001000,0x0009e000); //初始空闲内存，开头预留4M给系统
	//memeryFree(memeryList,0x00400000,memeryList->total_size - 0x00400000); //中间预留给显卡(?)
	//memeryList->total_free_size = 0;
	memeryList->number = 0;	//从0开始计数
	memeryList->max_number = 0;
	memeryList->lost_size = 0;
	memeryList->lost_number = 0;
} */

unsigned int memeryTotal(MEMERY_LIST *memeryList){ //获取空闲内存大小
	COUNT i;
	unsigned int totalSize = 0;
	for(i = 0;i < memeryList->number;i++){
		totalSize += memeryList->free[i].size;
	}
	memeryList->total_free_size = totalSize;
	return totalSize;
}

/* unsigned int memeryTotal(MEMERY_LIST *memeryList){
	unsigned int i, t = 0;
	for (i = 0; i < memeryList->number; i++) {
		t += memeryList->free[i].size;
	}
	return t;
} */




unsigned int memeryAlloc(MEMERY_LIST *memeryList,unsigned int size){ //内存分配
	int i; //计数器
	unsigned int addr; //分配到的首地址
	sortMemery(memeryList,0,memeryList->max_number); //内存块从大到小排序
	for(i = 0;i < memeryList->number;i++){
		if(memeryList->free[i].size >= size){ 
			addr = memeryList->free[i].addr;	//分配首地址为本块起始地址
			memeryList->free[i].addr += size; //本块地址偏移已分配大小
			memeryList->free[i].size -= size; //本块容量减去分配容量
			memeryList->total_free_size -= size; //总容量减去分配容量
			if(memeryList->free[i].size == 0){ //删除空块
				deleteBlock(memeryList,i);
			}
			sortMemery(memeryList,0,memeryList->number); //内存块从大到小排序
			return addr;
		}
	}
	return 0;
}

/* unsigned int memeryAlloc(MEMERY_LIST *memeryList,unsigned int size){
	unsigned int i, a;
	for (i = 0; i < memeryList->number; i++) {
		if (memeryList->free[i].size >= size) {
			a = memeryList->free[i].addr;
			memeryList->free[i].addr += size;
			memeryList->free[i].size -= size;
			if (memeryList->free[i].size == 0) {
				memeryList->number--;
				for (; i < memeryList->number; i++) {
					memeryList->free[i] = memeryList->free[i + 1]; 
				}
			}
			return a;
		}
	}
	return 0; 
} */
 
int memeryFree(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size){ //释放内存
	int i;
	sortAddr(memeryList,0,memeryList->number);
	for(i = 0;i < memeryList->number;i++){
		if(memeryList->free[i].addr > addr){
			break;
		}
	}

	if(i > 0){
		if(memeryList->free[i - 1].addr + memeryList->free[i - 1].size == addr){ //前有可用内存
			memeryList->free[i - 1].size += size;
			if(addr + size == memeryList->free[i].addr){
				memeryList->free[i - 1].size += memeryList->free[i].size;
				deleteBlock(memeryList,i);
			}
		}
		else{
			if(addr + size == memeryList->free[i].addr){
				memeryList->free[i].size += size;
				memeryList->free[i].addr = addr;
			}
			else{
				addBlock(memeryList,i);
				memeryList->free[i].size = size;
				memeryList->free[i].addr = addr;
				if(memeryList->max_number < memeryList->number){
					memeryList->max_number = memeryList->number; //更新历史最大块数
				}
			}
		}
		memeryList->total_free_size += size;
		sortMemery(memeryList,0,memeryList->number);
		return 0;		
	}
	
	if(i == 0){
		if(addr + size == memeryList->free[i].addr){
			memeryList->free[i].size += size;
			memeryList->free[i].addr = addr;
		}
		else{
			addBlock(memeryList,i);
			memeryList->free[i].addr = addr;
			if(memeryList->max_number < memeryList->number){
				memeryList->max_number = memeryList->number; //更新历史最大块数
			}			
		}
		memeryList->total_free_size += size;
		sortMemery(memeryList,0,memeryList->number);
		return 0;
	}
	
	if(i == memeryList->number - 1){
		if(memeryList->free[i].addr + memeryList->free[i].size == addr){
			memeryList->free[i].size += size;
		}
		else{
			memeryList->free[i+1].addr = addr;
			memeryList->free[i+1].size = size;
			memeryList->number++;
			if(memeryList->max_number < memeryList->number){
				memeryList->max_number = memeryList->number; //更新历史最大块数
			}
		}
		memeryList->total_free_size += size;
		sortMemery(memeryList,0,memeryList->number);
		return 0;
	}
	
	memeryList->lost_number++;
	memeryList->lost_size += size;
	sortMemery(memeryList,0,memeryList->number);
	return -1;
}


/* int memeryFree(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size){
	int i, j;
	for (i = 0; i < memeryList->number; i++) {
		if (memeryList->free[i].addr > addr) {
			break;
		}
	}
	if (i > 0) {
		if (memeryList->free[i - 1].addr + memeryList->free[i - 1].size == addr) {
			memeryList->free[i - 1].size += size;
			if (i < memeryList->number) {
				if (addr + size == memeryList->free[i].addr) {
					memeryList->free[i - 1].size += memeryList->free[i].size;
					memeryList->number--;
					for (; i < memeryList->number; i++) {
						memeryList->free[i] = memeryList->free[i + 1]; 
					}
				}
			}
			return 0; 
		}
	}

	if (i < memeryList->number) {
		if (addr + size == memeryList->free[i].addr) {
			memeryList->free[i].addr = addr;
			memeryList->free[i].size += size;
			return 0; 
		}
	}

	if (memeryList->number < MEMMAN_FREES) {
		for (j = memeryList->number; j > i; j--) {
			memeryList->free[j] = memeryList->free[j - 1];
		}
		memeryList->number++;
		if (memeryList->max_number < memeryList->number) {
			memeryList->max_number = memeryList->number;
		}
		memeryList->free[i].addr = addr;
		memeryList->free[i].size = size;
		return 0; 
	}
	memeryList->lost_number++;
	memeryList->lost_size += size;
	return -1; 
} */

unsigned int memeryAlloc4k(MEMERY_LIST *memeryList,unsigned int size){ //内存空间分配前向上取整，最小4K(0x)，减少碎片
	unsigned int addr; //分配到的首地址
	//size = (size + 0xfff) & BLOCK_ALIGN; //以4K为最小单位向上取整。相当于if((i & 0xfff) != 0){i = (i & 0xfffff000) + 0x1000;}
	size = (size + 0xfff) & 0xfffff000; //以4K为最小单位向上取整。相当于if((i & 0xfff) != 0){i = (i & 0xfffff000) + 0x1000;}
	addr = memeryAlloc(memeryList,size);	
}

 
/*  unsigned int memeryAlloc4k(MEMERY_LIST *memeryList,unsigned int size){
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memeryAlloc(memeryList, size);
	return a;
} */
 
unsigned int memeryFree4k(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size){ //内存空间释放按4K向上取整
	int i;
	size = (size + 0xfff) & 0xfffff000; //以4K为最小单位向上取整
	i = memeryFree(memeryList,addr,size);
	return i;
}

/* int memeryFree4k(MEMERY_LIST *memeryList,unsigned int addr,unsigned int size){
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memeryFree(memeryList, addr, size);
	return i;
} */


void deleteBlock(MEMERY_LIST *memeryList,int i){ //删除空内存表项
	int j;
	memeryList->number--;
	for(j = i;j < memeryList->number;j++){
		memeryList->free[j] = memeryList->free[j + 1];
	}
}

void addBlock(MEMERY_LIST *memeryList,int i){ //插入内存表项
	int j;
	for(j = memeryList->number;j > i;j--){
		memeryList->free[j] = memeryList->free[j - 1];
	}
	memeryList->number++;
}

/* void sortMemery(MEMERY_LIST *memeryList,int low,int high){ //空闲内存块按容量递增快速排序
	if(low > high){
		return;
	}
	int i = low;
	int j = high;
	FREE_BLOCK keyBlock = memeryList->free[low];
	
	while(i < j){
		while(i < j && keyBlock.size <= memeryList->free[j].size){
			j--;
		}
		memeryList->free[i] = memeryList->free[j];
		while(i < j && keyBlock.size >= memeryList->free[i].size){
			i++;
		}
		memeryList->free[j] = memeryList->free[i];
	}
	memeryList->free[i] = keyBlock;
	sortMemery(memeryList,low,high - 1);
	sortMemery(memeryList,i + 1,high);
} */

/* void sortAddr(MEMERY_LIST *memeryList,unsigned int low,unsigned int high){ //空闲内存块按地址递增快速排序
	if(low > high){
		return;
	}
	int i = low;
	int j = high;
	FREE_BLOCK keyBlock = memeryList->free[low];
	
	while(i < j){
		while(i < j && keyBlock.addr <= memeryList->free[j].addr){
			j--;
		}
		memeryList->free[i] = memeryList->free[j];
		while(i < j && keyBlock.addr >= memeryList->free[i].addr){
			i++;
		}
		memeryList->free[j] = memeryList->free[i];
	}
	memeryList->free[i] = keyBlock;
	sortAddr(memeryList,low,high - 1);
	sortAddr(memeryList,i + 1,high);
} */

void sortMemery(MEMERY_LIST *memeryList,int low,int high){ //按内存块大小递增简单排序
	COUNT i,j;
	FREE_BLOCK temp;
	for(i = 0;i < memeryList->number;i++){
		//temp = memeryList->free[i];
		for(j = i;j < memeryList->number;j++){
			if(memeryList->free[i].size > memeryList->free[j].size){
				temp = memeryList->free[j];
				memeryList->free[j] = memeryList->free[i];
				memeryList->free[i] = temp;
			}
		}
	}
}

void sortAddr(MEMERY_LIST *memeryList,unsigned int low,unsigned int high){ //按内存地址大小递增简单排序
	COUNT i,j;
	FREE_BLOCK temp;
	for(i = 0;i < memeryList->number;i++){
		for(j = i;j < memeryList->number;j++){
			if(memeryList->free[i].addr > memeryList->free[j].addr){
				temp = memeryList->free[j];
				memeryList->free[j] = memeryList->free[i];
				memeryList->free[i] = temp;
			}
		}
	}	
}

 



 