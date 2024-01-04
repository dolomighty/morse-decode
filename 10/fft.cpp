
#undef NDEBUG
#include <assert.h>

#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include "RING.h"
#include "main.h"
#include "COUNT.h"



static struct {
    fftwf_plan plan;
    int    len;     // 2ⁿ, es. 256
    float* window;
    float* reals;   // prerequisito del plan
    float* power;   // prerequisito del plan
    int    period;
    int    timeout;
    RING   ring;
} fft;




void fft_init( int fft_len, int period ){ // HEADER
    // fft_len = 2ⁿ
    // period, ogni quanti frames viene fatta la fft

    assert( fft_len > 0 );
    fft.len = fft_len;

    assert( period > 0 );
    fft.period = period;
    fft.timeout = 0;

    fft.ring.init(fft.len);

#define M(WHO) assert( WHO = (float*)fftwf_malloc( fft.len * sizeof(*WHO)))
    M( fft.reals );
    M( fft.power );
#undef M
    
    fft.plan = fftwf_plan_r2r_1d( fft.len, fft.reals, fft.power, FFTW_R2HC, FFTW_MEASURE );
    
#define M(WHO) assert( WHO = (float*)malloc( fft.len * sizeof( *WHO )))
    M( fft.window );
#undef M

    for( int i=0; i<fft.len; i++ ){
        fft.window[i] = (1-cos(i*2*M_PI/fft.len))/2;  // hanning
    }
}










float* fft_in( float v ){ // HEADER
    // chiamata per ogni frame
    // v = [-1,+1]
    // quando ha uno spettro pronto, ritorna un float* lungo fft.len/2
    // null altrimenti

    fft.ring.wr(v);
    fft.timeout--;
    if( fft.timeout > 0 )return 0;

    // it's fft time !!
    fft.timeout = fft.period;

    // snapshot+windowing
    float snapshot[fft.len];
    fft.ring.snap( snapshot, COUNT(snapshot));
    for( int i=0; i<fft.len; i++ ){
        fft.reals[i] = snapshot[i] * fft.window[i];
    }

//    // OPPURE
//    // snapshot, no windowing
//    fft.ring.snap( fft.reals, fft.len );

#define SQUARE(A) ((A)*(A))
    
    fftwf_execute( fft.plan );
    
    // http://www.fftw.org/doc/The-Halfcomplex_002dformat-DFT.html
    // ... real part in hc[k] and its imaginary part in hc[n-k] ...
    // da cui ricaviamo len/2 bins di power spectrum
    // se il client vuole le ampiesse, si fara le sqrt
    for( int i=1; i<fft.len/2; i++ ){
        fft.power[i] = SQUARE(fft.power[i])+SQUARE(fft.power[fft.len-i]);
    }

    return fft.power;
}


