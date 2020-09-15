#include "api.h"

void NNOSMain(){
	int fh;
	char c,command[30],*p;
	api_command(command, 30);
	p = command;
	while(*p > ' '){
		p++;
	}
	while(*p == ' '){
		p++;
	}
	fh = api_fopen(p);
	if (fh != 0){
		while(1){
			if (api_fread(&c,1,fh) == 0) {
				break;
			}
			api_printc(c);
		}
	}
	else{
		api_printl("File not found.\n");
	}
	api_return();
}