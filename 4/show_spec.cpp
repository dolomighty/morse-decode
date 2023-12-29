
#include "main.h"
#include <stdio.h>



void show_spec( float *spec ){ // HEADER
    const char* charset[] = {" ","▁","▂","▃","▄","▅","▆","▇","█"};
    const int M = COUNT(charset)-1; // dovrebbe risolverlo a compile time

    for( int i=0; i<80; i++ ){
        int v = spec[i]*2;
        if(v<0)v=0;
        if(v>M)v=M;
        fprintf(stderr,"%s",charset[v]);
    }
    fprintf(stderr,"\r");
}


