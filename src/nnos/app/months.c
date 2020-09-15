//#include <stdio.h>
#include "api.h"

void NNOSMain(){
    int m,i;
    printf("Please input a month number(1~31):\n");
    //m = (rand() % 30) + 1;
	m = 13;
    printf("Calendar 2006-12\n");
    printf("----------------------------------------------------------\n");
    printf("Su\tMo\tTu\tWe\tTh\tFr\tSa\n" );
    printf("----------------------------------------------------------\n");
    
    if(m == 1){
        printf("\t\t\t\t\t%d\t\n",m);
    }
    else if(m == 2){
        printf("\t\t\t\t\t\t%d\n",m);
    }
    else{
        i = (m - 2) % 7 - 1;
        while(i){
			printf("\t");
            i--;
        }
		printf("%d\n",m);
    }

    printf("----------------------------------------------------------\n");
	api_return();
}