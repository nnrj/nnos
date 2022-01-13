#include "api.h"

void NNOSMain(){
	int i, timer;
	timer = api_alloctimer();
	api_inittimer(timer, 128);
	for (i = 20000000; i >= 20000; i -= i / 100) { //20KHz~20Hz�C�l?�ߓ��I�����?
		api_beep(i);							   //i��1%�I���x??
		api_settimer(timer, 1);					   ////?�u?0.01�b
		if (api_getkey(1) != 128) {
			break;
		}
	}
	api_beep(0);
	api_return();
}
