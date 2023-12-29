
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <string.h>


// HEADERBEG
struct DELAY {
    float* samples;
    int    nsamples;
    int    head;
    DELAY( int l );
    float push( float in );
    float max();
};
// HEADEREND





DELAY::DELAY( int l ){
    nsamples = l;
    samples = (float*)calloc( nsamples, sizeof(samples[0]));
    head = 0;
}

float DELAY::push( float in ){
    float last = samples[head];
    samples[head] = in;
    head = (head+1)%nsamples;
    return last;
}

float DELAY::max(){
    float max = -1E9;
    for( int i=0; i<nsamples; i++ ){
        if(max<samples[i]) max=samples[i];
    }
    return max;
}
