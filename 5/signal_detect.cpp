
#include <stdio.h>
#include "kmeans.h"
#include "main.h"



//bool signal_detect( float *spec, int len ){       // HEADER
//    // proviamo con k-means unidimensionale
//    // due gruppi, noise floor e picchi
//    // dalle medie ricaviamo una midline su cui discriminare
//    static float means[2]={0,2}; // iniziale
////    kmeans_update( means, COUNT(means), spec, len );
////    fprintf(stderr,"\rnoise %.2f signal %.2f", means[0], means[1]);
//    float midline = (means[0]+means[1])/2;
//    for( int i=0; i<len; i++ ){
//        if( spec[i] > midline ) return true;
//    }
//    return false;
//}






bool signal_detect( float *spec, int len, float thresh ){       // HEADER
    // sembra che una semplice scansione per il picco sia sufficente
    for( int i=0; i<len; i++ ){
        if(spec[i]>thresh)return true;
    }
    return false;

//    // proviamo con una certa isteresi
//    static bool mark = false;
//    for( int i=0; i<len; i++ ){
//        if( mark && spec[i]<(thresh-1)) return (mark=false);
//        if(!mark && spec[i]>(thresh+1)) return (mark=true);
//    }
//    return mark;
}
