
#include "main.h"
#include <stdio.h>



void show_spec( float *spec, bool signal ){ // HEADER
    const char* charset[] = {" ","▁","▂","▃","▄","▅","▆","▇","█"};
    const int M = COUNT(charset)-1; // dovrebbe risolverlo a compile time

    fprintf(stderr,"\r");
    float max = -1E9;
    for( int i=0; i<60; i++ ){
        if(max<spec[i])max=spec[i];
        int v = spec[i];
        if(v<0)v=0;
        if(v>M)v=M;
        fprintf(stderr,"%s",charset[v]);
    }
    fprintf(stderr," %.2f %s  ",max,signal?"mark":"    ");
}


