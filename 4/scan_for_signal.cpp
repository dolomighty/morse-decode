
#include <stdio.h>
#include "kmeans.h"
#include "main.h"



bool scan_for_signal( float *spec, int len ){       // HEADER
    // proviamo con k-means unidimensionale
    // due gruppi, noise floor e picchi
    // dalle medie ricaviamo una midline su cui discriminare
    static float means[2]={0,2}; // iniziale
//    kmeans_update( means, COUNT(means), spec, len );
//    fprintf(stderr,"\rnoise %.2f signal %.2f", means[0], means[1]);
    float midline = (means[0]+means[1])/2;
    for( int i=0; i<len; i++ ){
        if( spec[i] > midline ) return true;
    }
    return false;
}





