#include "api.h"
//#include <bits/stdc++.h>
//#include <windows.h>
//#include <conio.h> ///非标准输入输出

void NNOSMain(){
    //Beep(698,1000); /// 控制计算机中的蜂鸣器 Beep(频率，声音的持续时间（毫秒）)
    //int p;
    //int i = 0;
	//int flag = 1;
    //printf("input 1234567\n");
    //while(flag == 1)
	while(1){
        //p = api_getkey(1);
        switch(api_getkey(1)){
        case '1':
            Beep(554,500);break;
        case '2':
            Beep(622,500);break;
        case '3':
            Beep(682,500);break;
        case '4':
            Beep(740,500);break;
        case '5':
            Beep(831,500);break;
        case '6':
            Beep(932,500);break;
        case '7':
            Beep(1046,500);break;
		case 0x0a:
			//flag = 0;
			api_beep(0);
			api_return();
			break;
        default:
            break;
        }
    }
    //exit(0);
	api_return();
}


void Beep(int v,int t){
	int i;
	int timer = api_alloctimer();
	api_inittimer(timer,128);
	
	
	//for (i = 10 * t; i >= 0; i -= i / 100) { //20KHz~20Hz，人类可听到的声音范围
	//while()
							   //i以1%的速度递减
		
	while(1){
		api_beep(200000 + v);
		api_settimer(timer,t * 10);
		api_beep(0);
		return;
	}
						////间隔为0.01秒
		/* if (api_getkey(1) != 128) {
			break;
		} */
	//}
	api_beep(0);
	//api_return();	
}
/*
**  【音符频率对应表 】
** 音符 频率/HZ 半周期/us(N)
**--------------------------------
** 低1DO 262 1908 #1DO# 277 1805
** 低2RE 294 1700 #2RE# 311 1608
** 低3MI 330 1516 #3MI# 340 1470
** 低4FA 349 1433 #4FA# 370 1350
** 低5SO 392 1276 #5SO# 415 1205
** 低6LA 440 1136 #6LA# 466 1072
** 低7SI 494 1012 #7SI# 524 0954
** 中1DO 523 0956 #1DO# 554 0903
** 中2RE 578 0842 #2RE# 622 0804
** 中3MI 659 0759 #3MI# 682 0733
** 中4FA 698 0716 #4FA# 740 0676
** 中5SO 784 0638 #5SO# 831 0602
** 中6LA 880 0568 #6LA# 932 0536
** 中7SI 988 0506 #7SI# 1046 478
** 高1DO 1046 478 #1DO# 1109 451
** 高2RE 1175 426 #2RE# 1245 402
** 高3MI 1318 372 #3MI# 1356 368
** 高4FA 1397 358 #4FA# 1480 338
** 高5SO 1568 319 #5S0# 1661 292
** 高6LA 1760 284 #6LA# 1865 268
** 高7SI 1976 253 #7SI# 2066 242
**---------------------------------
** ("#"表示半音,用于上升半个音)
*/
