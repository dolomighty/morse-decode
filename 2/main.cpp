
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "fft.h"

// early crash via assert
#undef NDEBUG
#include <assert.h>


#define FFT_LEN 256

SDL_AudioSpec have;





void on_new_spec( float *spec, int len ){

    static float spec_emav[FFT_LEN/2]={0};

    // aggiorniamo una moving average dello spettro
    for( int i=0; i<len; i++ ){
        spec_emav[i] = spec_emav[i] + (spec[i]-spec_emav[i])*0.01;
    }

    // ora vediamo se lo spettro istantaneo supera la emav + un certo epsilon
    // se si, abbiamo un segnale
    const float epsilon = 0.1;
    bool signal = false;
    for( int i=0; i<len; i++ ){
        if( spec[i] > spec_emav[i]+epsilon ){
            signal=true;
            break;
        }
    }

    // edge trigger
    static bool signal_pre = false;
    static Uint32 msec_pre = 0;
    if( !signal_pre && signal ){
        Uint32 msec = SDL_GetTicks();
        Uint32 len = msec-msec_pre;
        printf("mark  %d msecs\n",len);
        msec_pre = msec;
    }
    if( signal_pre && !signal ){
        Uint32 msec = SDL_GetTicks();
        Uint32 len = msec-msec_pre;
        printf("space %d msecs\n",len);
        msec_pre = msec;
    }
    signal_pre = signal;
}








void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){
        float *spec = fft_process(*frame/32768.0);
        if(spec) on_new_spec(spec,FFT_LEN/2);
    }
}




int main( int argc, char *argv[] ){ 

    fft_init(FFT_LEN);

    assert( 0 == SDL_Init(SDL_INIT_AUDIO));
    atexit(SDL_Quit);
    
    SDL_AudioSpec     want;
    SDL_AudioDeviceID dev;

    SDL_zero(want);
    want.freq     = 11025;
    want.format   = AUDIO_S16;
    want.channels = 1;
    want.samples  = 256;
    want.callback = MyAudioCallback;

    assert( dev = SDL_OpenAudioDevice(0,SDL_TRUE,&want,&have,0));
    
    SDL_PauseAudioDevice(dev,0);  // start audio activity
//    while(SDL_GetTicks()<10000)
    while(1)
    {
//        int a = rms_min*130;
//        int b = rms_max*130;
//        int i=0;
//        while( i++ < a )putchar('.');
//        while( i++ < b )putchar('=');
//        putchar('\n');

//        int a = (rms_min+rms_max)/2*130;
//        int i=0;
//        while( i++ < a )putchar('=');
//        putchar('\n');

//        if(signal)putchar('=');
//        putchar('\n');

        fflush(stdout);
        SDL_Delay(100);
    }
    SDL_CloseAudioDevice(dev);

    return 0;
}


