
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "kmeans.h"

#undef NDEBUG
#include <assert.h>


#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))
#define CLAMP(WHAT,LO,HI) { if(WHAT<(LO))WHAT=(LO); else if(WHAT>(HI))WHAT=(HI); }


const int SAMPLERATE = 11025;

SDL_AudioSpec have;



//volatile float rms_min = 0;
//volatile float rms_max = 0;




const char* intensity( float v ){
    // v = [0,1]
    const char* charset[] = {" ","▁","▂","▃","▄","▅","▆","▇","█"};
    int i = v*COUNT(charset);
    CLAMP(i,0,COUNT(charset)-1);
    return charset[i];

}


//void rms_in( float rms ){
//    // chiamata ogni frame
//    // rms [0,1]
//    // creiamo un minmax tramite una emav
//
//    // min e max tendono contro-esponenzialmente a rms
//    rms_min = rms_min + (rms-rms_min)*0.01;
//    rms_max = rms_max + (rms-rms_max)*0.01;
//
//    // se troviamo un nuovo minmax, lo ricordiamo
//    if(rms_min>rms)rms_min=rms;
//    if(rms_max<rms)rms_max=rms;
//
//    // ora possiamo binarizzare rms
//    // passiamo anche qui da una emav
//    float mid = (rms_max+rms_min)/2;
//    static float mid_emav = 0;
//    mid_emav = mid_emav + (mid-mid_emav)*0.01;
//    char signal = rms > mid_emav;
//
////    // edge trigger
////    static char signal_prev = 0;
////    if( !signal_prev && signal ){
////        // silenzio → segnale
////        fprintf(stderr,"·");
////    }
////    if( signal_prev && !signal ){
////        // segnale → silenzio
////        fprintf(stderr," ");
////    }
////    signal_prev = signal;
//}




volatile float g_level;
volatile bool g_signal;


//void rms_in( float rms ){
//    // chiamata ogni frame
//    // rms [0,1]
//    // creiamo un minmax tramite una emav
//
//    static int frame = 0;
//    frame++;
//
////    // lowpass
////    static float lp[5];
////    memmove( &lp[1], &lp[0], sizeof(lp[1])*(COUNT(lp)-1));
////    lp[0]=rms;
////    rms = (lp[0]+lp[1]+lp[2])/3;
//
//    // uno storico come base statistica
//    static float hist[20];
//    memmove( &hist[1], &hist[0], sizeof(hist[1])*(COUNT(hist)-1));
//    hist[0]=rms;
//
//    // min ave max
//    float min = +1E9; // +inf
//    float avg = 0;
//    float max = -1E9; // -inf
//    for( int i=0; i<COUNT(hist); i++ ){
//        if(min>hist[i]) min=hist[i];
//        if(max<hist[i]) max=hist[i];
//        avg += hist[i];
//    }
//    avg /= COUNT(hist);
//    g_min = min;
//    g_avg = avg;
//    g_max = max;
//
////    const float epsilon = 0.01;
////    bool signal = max > avg+epsilon;
//
//    // boh, sembra che una semplice soglia sia sufficente
//    bool signal = max > 0.01;
//    g_signal = signal;
//
//
//    // edge trigger
//    static bool signal_prev = false;
//    if( signal_prev && !signal ){
//        // segnale → silenzio = mark
//        fprintf(stderr,"%8d mark  %.2f %.2f %.2f\n",frame,min,avg,max);
//    }
//    if( !signal_prev && signal ){
//        // silenzio → segnale = space
//        fprintf(stderr,"%8d space %.2f %.2f %.2f\n",frame,min,avg,max);
//    }
//    signal_prev = signal;
//}









void rms_in( float rms ){
    // chiamata ogni frame
    // rms [0,1]
    // creiamo un minmax tramite una emav

    static int frame = 0;
    frame++;

    static float lp = 0;
    lp = lp + (rms-lp)*0.01;

    const int hist_len_msec = 10;
    static float hist[SAMPLERATE*hist_len_msec/1000];
    memmove( &hist[1], &hist[0], sizeof(hist[1])*(COUNT(hist)-1));
    hist[0]=lp;

    float max = -1E9;
    for( int i=0; i<COUNT(hist); i++ ){
        if(max<hist[i])max=hist[i];
    }
    g_level = max;

//    // one pole lowpass
//    static float lp = 0;
//    lp = lp+(rms-lp)*0.1;
//    g_level = lp;



    bool signal = max > 0.01;
    g_signal = signal;

    // edge trigger
    static bool signal_prev = false;
    if( signal_prev && !signal ){
        // segnale → silenzio = mark
        fprintf(stderr,"%8d mark\n",frame);
//        fprintf(stderr,"=");
    }
    if( !signal_prev && signal ){
        // silenzio → segnale = space
        fprintf(stderr,"%8d space\n",frame);
//        fprintf(stderr," ");
    }
    signal_prev = signal;
}









void MyAudioCallback( void *userdata , Uint8 *stream , int len_bytes ){
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames-- , frame++ ){
        float v = *frame/32768.0;
        rms_in(v*v);
    }
}




int main( int argc , char *argv[] ){ 

    assert(0==SDL_Init(SDL_INIT_AUDIO));    
    atexit(SDL_Quit);
    
    SDL_AudioSpec     want;
    SDL_AudioDeviceID dev;

    SDL_zero(want);
    want.freq     = SAMPLERATE;
    want.format   = AUDIO_S16;
    want.channels = 1;
    want.samples  = 256;
    want.callback = MyAudioCallback;

    assert(dev = SDL_OpenAudioDevice(0,SDL_TRUE,&want,&have,0));

    fprintf(stderr,"running\n");
    SDL_PauseAudioDevice(dev,0);  // start audio activity
    while(1){
//        char bar[80]="";
//        memset(bar,' ',COUNT(bar));
//        bar[COUNT(bar)-1]=0;
//
//        {
//            int x = g_min*500;
//            CLAMP(x,0,COUNT(bar)-1);
//            bar[x]='[';
//        }
//
//        {
//            int x = g_avg*500;
//            CLAMP(x,0,COUNT(bar)-1);
//            bar[x]='|';
//        }
//
//        {
//            int x = g_max*500;
//            CLAMP(x,0,COUNT(bar)-1);
//            bar[x]=']';
//        }
//
//        fprintf(stderr,"\r%s",bar);

//        fprintf(stderr,"\r%.2f %.2f %.2f %s   ",g_min,g_avg,g_max,g_signal?"mark":"space");

//        fprintf(stderr,"%f\n",g_level);
//        fprintf(stderr,"%s",g_signal?"=":" ");
//        fprintf(stderr,"%s",intensity(g_level));

        SDL_Delay(100);
    }
    SDL_CloseAudioDevice(dev);

    return 0;
}


