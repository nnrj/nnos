#include "api.h"

void NNOSMain(){
	int fh;
	char c;
	fh = api_fopen("test.txt");
	if (fh != 0){
		while(1){
			if(api_fread(&c,1,fh) == 0) {
				break;
			}
			api_printc(c);
		}
	}
	api_return();
}
