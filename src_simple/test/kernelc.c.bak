void HariMain(void){
	int i=0;
	for (i=0;i<=10;i++){
		*(char *)(0xb8000+12*160+i)='c';
		*(char *)(0xb8000+12*160+i+1)=0x0c;
		i++;
    }
	stop:
	goto stop;
}