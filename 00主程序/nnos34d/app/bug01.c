void api_printc(int c);
void api_return();

void NNOSMain(){
	char a[100];
	a[10] = 'A';
	api_printc(a[10]);
	a[102] = 'B';		//数组越界
	api_printc(a[102]); 
	a[123] = 'C';		//数组越界
	api_printc(a[123]); 
	api_return();
}