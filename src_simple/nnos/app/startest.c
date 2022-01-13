#include <stdio.h>
#include "api.h"

void NNOSMain(){
    int i,j,n,m;

    n = 5;
    m = 2;

    for(i = 1;i <= n;i++){
        for(j = (n - 1) - i;j >= 0;j--){
             printf(" ");
        }
        for(j = i * 2 - 1;j > 0;j--){
             printf("*");
        }
        for(j = 0;j < m;j++){
             printf(" ");
        }
        for(j = 2 * (n - i) + 1;j > 0;j--){
             printf("*");
        }
        printf("\n");
    }
	api_return();
}