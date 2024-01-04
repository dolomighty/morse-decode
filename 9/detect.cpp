
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "COUNT.h"
#include "audio.h"
#include "linreg.h"
#include "DELAY.h"
#include <vector>  // HEADER



#define NDELTA 50

// qui teniamo una lista delle durate
// verrà poi usata da k-means o un'altro algo di clustering
// per classificare punti, linee e spazi in base alla durata
std::vector<int> mark_delta;  // HEADER
std::vector<int> space_delta; // HEADER





float detect_eps = 0.02; // HEADER



float sigmoid( float t ){
    float e = exp(t);
    return e/(1+e);
}




float integ_filt( float in ){
    static float y;
    static float dy;
    dy += (in-y)*0.01;
    dy *= 0.99;
    y += dy*0.01;
    return y;
}




bool on_mark_change( bool mark, int frame ){

    static bool prev_mark;
    static int  prev_frame;
    static int  idx;

    // edge sense
    if(!prev_mark && mark){
        // space → mark
        int delta = frame-prev_frame;
        fprintf(stderr,"space %d\n",delta);
        // salviamo
        if(0==space_delta.size())space_delta.resize(NDELTA);
        space_delta[idx%space_delta.size()]=delta;
        idx++;
    } else
    if(prev_mark && !mark){
        // mark → space
        int delta = frame-prev_frame;
        fprintf(stderr,"mark  %d\n",delta);
        // salviamo
        if(0==mark_delta.size())mark_delta.resize(NDELTA);
        mark_delta[idx%mark_delta.size()]=delta;
        idx++;
    }

    prev_mark  = mark;
    prev_frame = frame;
    return mark;
}





static float beep(){
    static float v = 0.5;
    return (v = -v);
}


static float shift( float buf[], int nbuf, float in ){
    const int end = nbuf-1;
    float last = buf[0];
    memmove( &buf[0], &buf[1], end*sizeof(buf[1]));
    buf[end] = in;
    return last;
}


static float integ( float buf[], int nbuf ){
    float sum = 0;
    for( int i=0; i<nbuf; i++ ){
        sum += buf[i];
    }
    return sum;
}





//float detect( float in ){  // HEADER
//    // in = [-1,+1]
//
//    float v = abs(in);
//
//    static float peak;
//    if(peak<v)peak=v;
//    else
//    peak -= peak*0.01;
////    return peak;
//
//    static float hist1[FRAMES_FROM_MSEC( 10 )];
//    static float hist2[FRAMES_FROM_MSEC( 20 )];
//
//    shift( hist1, COUNT(hist1), peak );
//    shift( hist2, COUNT(hist2), peak );
//
//    float avg1 = integ( hist1, COUNT(hist1))/COUNT(hist1);
//    float avg2 = integ( hist2, COUNT(hist2))/COUNT(hist2);
//
//    static bool mark;
//
//    if(!mark && avg1 > avg2+detect_eps) mark=true;
//    if( mark && avg1 < avg2-detect_eps) mark=false;
//
////    return mark ? 1 : 0;
//    return avg2;
//}


float detect( float in ){  // HEADER
    // in = [-1,+1]

    static int frame;
    frame++;

//    float v = in*in;
    float v = abs(in);

//    static float peak;
//    peak *= 0.99;
//    if(peak<v)peak=v;
////    return sigmoid(peak*10);

    static DELAY hist(FRAMES_FROM_MSEC( 10 ));
    hist.push( v );
    float peak = hist.max();
//    return sigmoid(peak*10);
//    audio_aux_red( peak*5 );
//    audio_aux_red( integ_filt(peak)*5 );
    

    static DELAY delay(FRAMES_FROM_MSEC( 10 ));
    float peak_delayed = delay.push( peak );
//    return peak_delayed;

    static float fast;
    fast += (peak-fast)*0.005;
//    fast += (peak_delayed-fast)*0.01;
//    return fast;

    static float slow;
    slow += (peak-slow)*0.001;
//    return slow;

    audio_aux_red( fast*5 );
    audio_aux_grn( slow*5 );
//    audio_aux_grn( fast*5/(slow*5+1));
//    return slow*5;

//    static bool mark;
//    if(!mark && fast > slow+detect_eps) mark = on_mark_change( true,  frame );
//    if( mark && fast < slow-detect_eps) mark = on_mark_change( false, frame );
//    return (mark ? beep() : 0)*0.3 -0.6;

    static bool mark;
    float d = fast-slow;
    if(!mark && d > +detect_eps ) mark = on_mark_change( true,  frame );
    if( mark && d < -detect_eps ) mark = on_mark_change( false, frame );
    return (mark ? beep() : 0)*0.3 -0.6;
}   




//float detect( float in ){  // HEADER
//    // in = [-1,+1]
//
//    float sq = in*in;
//
//    static int frame;
//    frame++;
//
//    static float delay[FRAMES_FROM_MSEC( 15 )];
//    static float hist1[FRAMES_FROM_MSEC( 10 )];
//    shift( hist1, COUNT(hist1), shift( delay, COUNT(delay), sq ));
//    float fast = integ( hist1, COUNT(hist1))/COUNT(hist1);
//
//    static float hist2[FRAMES_FROM_MSEC( 40 )];
//    shift( hist2, COUNT(hist2), sq );
//    float slow = integ( hist2, COUNT(hist2))/COUNT(hist2);
//
//    audio_aux_red( fast*20 );
//    audio_aux_grn( slow*20 );
////    return slow*20;
//
//    static bool mark;
//    if(!mark && fast > slow+detect_eps) mark = on_mark_change( true,  frame );
//    if( mark && fast < slow-detect_eps) mark = on_mark_change( false, frame );
//    return mark ? beep() : 0;
//
////    static bool mark;
////    float d = fast-slow;
////    if( d > +detect_eps ) mark = on_mark_change( true,  frame );
////    if( d < -detect_eps ) mark = on_mark_change( false, frame );
////    return mark ? beep() : 0;
//}   



