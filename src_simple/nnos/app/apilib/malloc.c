#include "api.h"

void *malloc(int size){
	char *p = api_malloc(size + 16); //多分配16字节，存放size值，size本身只占4字节，但大小为16倍数时CPU处理更快
	if(p != 0){
		*((int *)p) = size;
		p += 16;
	}
	return p;
}

