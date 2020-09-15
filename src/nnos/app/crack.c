//内存病毒
void api_return();

void NNOSMain(){
	*((char *) 0x00102600) = 0;
	//return;
	api_return();
}
