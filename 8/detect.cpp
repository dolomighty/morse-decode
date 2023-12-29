
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include "COUNT.h"
#include "audio.h"





static void shift_in( float buf[], int nbuf, float in ){
    const int end = nbuf-1;
    memmove( &buf[0], &buf[1], end*sizeof(buf[1]));
    buf[end] = in;
}


static float integ( float buf[], int nbuf ){
    float sum = 0;
    for( int i=0; i<nbuf; i++ ){
        sum += buf[i];
    }
    return sum;
}




float detect_eps = 0.05; // HEADER



float detect( float in ){  // HEADER
    // in = [-1,+1]

    float v = abs(in);

    static float peak;
    if(peak<v)peak=v;
    else
    peak -= peak*0.01;
//    return peak;

    static float hist1[ 10 /* msec */ *SAMPLE_RATE/1000];
    static float hist2[ 20 /* msec */ *SAMPLE_RATE/1000];

    shift_in( hist1, COUNT(hist1), peak );
    shift_in( hist2, COUNT(hist2), peak );

    float avg1 = integ( hist1, COUNT(hist1))/COUNT(hist1);
    float avg2 = integ( hist2, COUNT(hist2))/COUNT(hist2);

    static bool mark;

    if(!mark && avg1+detect_eps > avg2) mark=true;
    if( mark && avg1-detect_eps < avg2) mark=false;

    return mark ? 1 : 0;
//    return avg2;
}


