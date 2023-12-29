

#include <fftw3.h>
// wget https://fftw.org/fftw-3.3.10.tar.gz
// tar -xf fftw-3.3.10.tar.gz
// cd fftw-3.3.10
// sh bootstrap.sh --enable-float
// make
// sudo make install

#include <math.h>
#include <stdlib.h>
#include "ring.h"
#include "main.h"

// early crash via assert
#undef NDEBUG
#include <assert.h>



static struct {
    fftwf_plan plan;
    int   len; // 2ⁿ, es. 256
    float *window;
    float *reals; // prerequisito del plan
    float *spect; // prerequisito del plan
    int   period;
    int   timeout;
} fft;




void fft_init( int fft_len, int period ){ // HEADER
    // fft_len = 2ⁿ
    // period, ogni quanti frames viene fatta la fft

    assert( fft_len > 0 );
    fft.len = fft_len;

    assert( period > 0 );
    fft.period = period;
    fft.timeout = 0;

    ring_init(fft.len);

#define M(WHO) assert( WHO = (float*)fftwf_malloc( fft.len * sizeof(*WHO)))
    M( fft.reals );
    M( fft.spect );
#undef M
    
    fft.plan = fftwf_plan_r2r_1d( fft.len, fft.reals, fft.spect, FFTW_R2HC, FFTW_MEASURE );
    
#define M(WHO) assert( WHO = (float*)malloc( fft.len * sizeof( *WHO )))
    M( fft.window );
#undef M

    for( int i=0; i<fft.len; i++ ){
        fft.window[i] = (1-cos(i*2*M_PI/fft.len))/2;  // hanning
    }
}










float *fft_in( float v ){ // HEADER
    // chiamata per ogni frame
    // v = [-1,+1]
    // quando ha uno spettro pronto, ritorna un float* lungo fft.len/2
    // null altrimenti

    ring_wr(v);
    fft.timeout--;
    if( fft.timeout > 0 )return 0;

    // it's fft time !!
    fft.timeout = fft.period;

    float snapshot[fft.len];
    ring_snap( snapshot, COUNT(snapshot));

    // windowing
    for( int i=0; i<fft.len; i++ ){
        fft.reals[i] = snapshot[i] * fft.window[i];
    }

#define SQUARE(A) ((A)*(A))
    
    fftwf_execute( fft.plan );
    
    // http://www.fftw.org/doc/The-Halfcomplex_002dformat-DFT.html
    // ... real part in hc[k] and its imaginary part in hc[n-k] ...
    // da cui ricaviamo len/2 bins di power spectrum
    for( int i=1; i<fft.len/2; i++ ){
//        fft.spect[i] = SQUARE( fft.spect[i] ) + SQUARE( fft.spect[fft.len-i] );
        fft.spect[i] = sqrt( SQUARE( fft.spect[i] ) + SQUARE( fft.spect[fft.len-i] ));
    }
//  fft.spect[0] = SQUARE( fft.spect[0] );

    return fft.spect;
}


