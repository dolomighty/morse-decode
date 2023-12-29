
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "COUNT.h"
#include "main.h"
#include "detect.h"
#include <queue> // HEADER


#define AUDIO_FPS 22050        // HEADER

#define FRAMES_FROM_MSEC(MSEC) ((MSEC)*AUDIO_FPS/1000) // HEADER


static SDL_AudioSpec     have;
static SDL_AudioDeviceID dev;



// HEADERBEG
struct AUDIO_STATS {
    float imin, imax; // input
    float omin, omax; // output
    float rmin, rmax; // aux track red
    float gmin, gmax; // aux track green
};
// HEADEREND

std::queue<AUDIO_STATS> audio_stats; // HEADER

int audio_stats_msec = 10; // HEADER

bool audio_paused = false; // HEADER

bool audio_aux_red_enabled = false; // HEADER
bool audio_aux_grn_enabled = false; // HEADER
float aux_red = 0;
float aux_grn = 0;

float audio_aux_red( float v ){ // HEADER
    audio_aux_red_enabled=true;
    return (aux_red=v);
} 

float audio_aux_grn( float v ){ // HEADER
    audio_aux_grn_enabled=true;
    return (aux_grn=v);
} 





void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){
    if(audio_paused)return;
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){

        float i = *frame/32768.0;
        float o = detect( i );

        static AUDIO_STATS stats;
        if( stats.imin > i       )stats.imin = i;
        if( stats.imax < i       )stats.imax = i;
        if( stats.omin > o       )stats.omin = o;
        if( stats.omax < o       )stats.omax = o;
        if( stats.rmin > aux_red )stats.rmin = aux_red;
        if( stats.rmax < aux_red )stats.rmax = aux_red;
        if( stats.gmin > aux_grn )stats.gmin = aux_grn;
        if( stats.gmax < aux_grn )stats.gmax = aux_grn;

        static int timeout;
        timeout--;
        if(timeout>0)continue;
        timeout = FRAMES_FROM_MSEC(audio_stats_msec);

        audio_stats.push(stats);
        stats.imin = +1E9;
        stats.imax = -1E9;
        stats.omin = +1E9;
        stats.omax = -1E9;
        stats.rmin = +1E9;
        stats.rmax = -1E9;
        stats.gmin = +1E9;
        stats.gmax = -1E9;
    }
}





void audio_init(){  // HEADER

    assert( 0 == SDL_Init(SDL_INIT_AUDIO));
    atexit(SDL_Quit);
    
    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq     = AUDIO_FPS;
    want.format   = AUDIO_S16;
    want.channels = 1;
    want.samples  = 256;
    want.callback = MyAudioCallback;

    assert( dev = SDL_OpenAudioDevice( 0, SDL_TRUE, &want, &have, 0 ));
}






void audio_start(){  // HEADER
    SDL_PauseAudioDevice(dev,0);  // start audio activity
}







void audio_stop(){  // HEADER
    SDL_CloseAudioDevice(dev);
}



