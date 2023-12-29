

#include <fftw3.h>
// wget https://fftw.org/fftw-3.3.10.tar.gz
// tar -xf fftw-3.3.10.tar.gz
// cd fftw-3.3.10
// sh bootstrap.sh --enable-float
// make
// sudo make install

#include <math.h>
#include <stdlib.h>

// early crash via assert
#undef NDEBUG
#include <assert.h>


static int FFT_LEN = 0;

// per l'overlap serve un ring buffer snapshottabile
// #define FFT_OVERLAP 100 

static float *fft_stream;
static float *fft_window;
static float *fft_reals;
static float *fft_spect;
static fftwf_plan fft_plan;
//static int *fft_power;




void fft_init( int len ){ // HEADER
    assert(len>0);
    FFT_LEN = len;
    
//    assert( fft_power = malloc( WID * sizeof( *fft_power )));
    
#define M(WHO) assert( WHO = (float*)malloc( FFT_LEN * sizeof( *WHO )))
    M( fft_stream );
    M( fft_window );
#undef M

#define M(WHO) assert( WHO = (float*)fftwf_malloc( FFT_LEN * sizeof(*WHO)))
    M( fft_reals );
    M( fft_spect );
#undef M
    
    fft_plan = fftwf_plan_r2r_1d( FFT_LEN, fft_reals, fft_spect, FFTW_R2HC, FFTW_MEASURE );
    
    for( int i=0; i<FFT_LEN; i++ ){
        fft_stream[i] = 0;
        fft_window[i] = (1-cos(i*2*M_PI/FFT_LEN))/2;  // hanning window
    }
}







float *fft_process( float v ){ // HEADER
    // chiamata per ogni frame
    // v = [-1,+1]
    // quando ha uno spettro pronto, ritorna un float* lungo FFT_LEN/2 
    // null altrimenti
    
    // buffering
    static float *p = fft_stream;
    *p++ = v;
    if( p < fft_stream+FFT_LEN )return 0;
    // wraparound e processing del buffer
    p = fft_stream;
    
    // windowing
    for( int i=0; i<FFT_LEN; i++ ){
        fft_reals[i] = fft_stream[i] * fft_window[i];
    }

#define SQUARE(A) ((A)*(A))
    
    // p prima = p dopo, perche wrappa a FFT_LEN
    
    fftwf_execute( fft_plan );
    
//    // log power, lin freq
//    for( i=0; i<WID; i++ ){
//        int a = (i+0) * (FFT_LEN/2) / WID;
//        int b = (i+1) * (FFT_LEN/2) / WID;
//        fft_power[i]=0;
//        for (; a<b; a++ ){
//            int p = HGT * 0.1 * log( 1 + sqrt( SQUARE( fft_spect[a] ) + SQUARE( fft_spect[FFT_LEN-1-a] )));
//            if( fft_power [i] < p ) fft_power [i] = p;
//        }
//    }
    
    
    // http://www.fftw.org/doc/The-Halfcomplex_002dformat-DFT.html
    // ... real part in hc[k] and its imaginary part in hc[n-k] ...
    // power spectrum
    for( int i=1; i<FFT_LEN/2; i++ ){
//    fft_spect[i] = SQUARE( fft_spect[i] ) + SQUARE( fft_spect[FFT_LEN-i] );
        fft_spect[i] = sqrt( SQUARE( fft_spect[i] ) + SQUARE( fft_spect[FFT_LEN-i] ));
    }
//  fft_spect[0] = SQUARE( fft_spect[0] );

    return fft_spect;
}





