


#include "main.h"
#include <math.h>
#include <string.h>


float ASDF( float a[], float b[], int n ){
    float ddsum = 0;
    for( int i=0; i<n; i++ ){
        float d = a[i]-b[i];
        ddsum += d*d; 
    }
    return ddsum;
//    return sqrt(ddsum)/n;
}


const int MAX_LAGS = 1000;

float* acorr_in( float in, int lags ){ // HEADER
    // calcola l'autocorrelazione via ASDF ogni period samples
    // i periodi appaiono quindi come zeri in out

    static float out[MAX_LAGS];

    // coda
    static float samples[MAX_LAGS*2]; // per calcolare i lags richiesti servono lags*2 samples
    memmove( &samples[1], &samples[0], sizeof(samples[1])*(lags*2-1));
    samples[0]=in;

    static int timeout = 0;
    timeout--;
    if(timeout>0)return 0;
    timeout = lags;

    for( int lag=0; lag<lags; lag++ ){
        out[lag] = ASDF( &samples[0], &samples[lag], lags );
    }

    return out;
}

