
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
    float cmin, cmax; // aux track cyan
    void reset();
    void update( float i, float o, float r, float g, float c );
};
// HEADEREND

void AUDIO_STATS::reset(){
    imin = +1E9;
    imax = -1E9;
    omin = +1E9;
    omax = -1E9;
    rmin = +1E9;
    rmax = -1E9;
    gmin = +1E9;
    gmax = -1E9;
    cmin = +1E9;
    cmax = -1E9;
}

void AUDIO_STATS::update( float i, float o, float r, float g, float c ){
    imin = std::min(imin,i);
    imax = std::max(imax,i);
    omin = std::min(omin,o);
    omax = std::max(omax,o);
    rmin = std::min(rmin,r);
    rmax = std::max(rmax,r);
    gmin = std::min(gmin,g);
    gmax = std::max(gmax,g);
    cmin = std::min(cmin,c);
    cmax = std::max(cmax,c);
}


bool audio_running = false; // HEADER


std::queue<AUDIO_STATS> audio_stats; // HEADER

int audio_stats_msec = 10; // HEADER

bool audio_paused = false; // HEADER

bool audio_aux_red_enabled = false; // HEADER
bool audio_aux_grn_enabled = false; // HEADER
bool audio_aux_cyn_enabled = false; // HEADER
float aux_red = 0;
float aux_grn = 0;
float aux_cyn = 0;

float audio_aux_red( float v ){ // HEADER
    audio_aux_red_enabled=true;
    return (aux_red=v);
} 

float audio_aux_grn( float v ){ // HEADER
    audio_aux_grn_enabled=true;
    return (aux_grn=v);
} 

float audio_aux_cyn( float v ){ // HEADER
    audio_aux_cyn_enabled=true;
    return (aux_cyn=v);
} 


float softknee( float in ){
    if(in>0) return in/(1+in);
    in = -in;
    return -in/(1+in);
}


void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){
    audio_running=true;
    if(audio_paused)return;
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){

        float i = *frame/32768.0;
        float o = detect( i );

        static AUDIO_STATS stats;
        stats.update( i, o, aux_red, aux_grn, aux_cyn );

        static int timeout;
        timeout--;
        if(timeout>0)continue;
        timeout = FRAMES_FROM_MSEC(audio_stats_msec);

        audio_stats.push(stats);
        stats.reset();
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

    audio_running = false;
}






void audio_start(){  // HEADER
    SDL_PauseAudioDevice(dev,0);  // start audio activity
}







void audio_stop(){  // HEADER
    SDL_CloseAudioDevice(dev);
}



