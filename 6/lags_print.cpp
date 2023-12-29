
#include "main.h"
#include <stdio.h>



void lags_print( float *lags, int n ){ // HEADER
    const char* charset[] = {" ","▁","▂","▃","▄","▅","▆","▇","█"};
    const int M = COUNT(charset)-1; // dovrebbe risolverlo a compile time

    fprintf(stderr,"\r");
    for( int i=0; i<60; i++ ){
        int v = lags[i];
        if(v<0)v=0;
        if(v>M)v=M;
        fprintf(stderr,"%s",charset[v]);
    }
}


