
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "fft.h"

// early crash via assert
#undef NDEBUG
#include <assert.h>



#define FFT_LEN 256

SDL_AudioSpec have;


#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))




static float spec_emav[FFT_LEN/2]={0};




void show_spec( float *spec ){
    const char* charset[] = {" ","▁","▂","▃","▄","▅","▆","▇","█"};
    const int M = COUNT(charset)-1; // dovrebbe risolverlo a compile time

    for( int i=0; i<80; i++ ){
        int v = spec[i]*2;
        if(v<0)v=0;
        if(v>M)v=M;
        printf("%s",charset[v]);
    }
    printf("\r");
    fflush(stdout);
}





void on_new_spec( float *spec, int len ){

//    show_spec( spec );


    // cerchiamo un picco sopra una soglia
    // sembra in buona parte sufficente
    const float epsilon = 5;
    bool signal = false;
    for( int i=0; i<len; i++ ){
        if( spec[i] > epsilon ){
            signal=true;
            break;
        }
    }




//    // aggiorniamo una emav totale sullo spettro
//    static float spec_avg = 0;
//    for( int i=0; i<len; i++ ){
//        spec_avg = spec_avg + (spec[i]-spec_avg)*0.01;
//    }
//
//    // molto basico, cerchiamo un picco sopra una soglia
//    const float epsilon = 0.01;
//    bool signal = false;
//    for( int i=0; i<len; i++ ){
//        if( spec[i] > spec_avg+epsilon ){
//            signal=true;
//            break;
//        }
//    }


//    // aggiorniamo una emav per ogni bin
//    // l'idea è di applicare uno smoothing teporale ai picchi
//    // per alzare il SNR
//    for( int i=0; i<len; i++ ){
//        spec_emav[i] = spec_emav[i] + (spec[i]-spec_emav[i])*0.5;
//    }
//    // cerchiamo un picco sopra una soglia
//    const float epsilon = 4;
//    bool signal = false;
//    for( int i=0; i<len; i++ ){
//        if( spec_emav[i] > epsilon ){
//            signal=true;
//            break;
//        }
//    }


//    // aggiorniamo una emav per ogni bin
//    for( int i=0; i<len; i++ ){
//        spec_emav[i] = spec_emav[i] + (spec[i]-spec_emav[i])*0.1;
//    }
//
//    // ora cerchiamo il primo bin che supera la emav + un certo epsilon
//    // se si, abbiamo un segnale
//    const float epsilon = 2;
//    bool signal = false;
//    for( int i=len-1; i>=0; i-- ){
//        if( spec[i] > spec_emav[i]+epsilon ){
//            signal=true;
//            break;
//        }
//    }





    // edge trigger
    // genera i mark e space con relativo timing
    // questi poi devon passare nel discriminatore punto-linea
    // e da li, dizionario
    static bool signal_pre = false;
    static Uint32 msec_pre = 0;
    if( !signal_pre && signal ){
        // space → mark
        Uint32 msec = SDL_GetTicks();
        Uint32 len = msec-msec_pre;
        msec_pre = msec;
        printf("%4d msecs mark\n",len);
        fflush(stdout);
    }
    if( signal_pre && !signal ){
        // mark → space
        Uint32 msec = SDL_GetTicks();
        Uint32 len = msec-msec_pre;
        msec_pre = msec;
        printf("%4d msecs space\n",len);
        fflush(stdout);
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

//        show_spec();
//
//        fflush(stdout);
        SDL_Delay(200);
    }
    SDL_CloseAudioDevice(dev);

    return 0;
}


