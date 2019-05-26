#include "api.h"

void free(void *p){
	char *q = p;
	int size;
	if(q != 0){
		q -= 16;
		size = *((int *)q); //取出size值，释放size + 16大小的空间
		api_free(q,size + 16);
	}
}

