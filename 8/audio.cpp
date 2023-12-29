
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "COUNT.h"
#include "main.h"
#include "detect.h"


#define SAMPLE_RATE 22050        // HEADER


static SDL_AudioSpec     have;
static SDL_AudioDeviceID dev;


volatile float in_min;
volatile float in_max;
volatile float out_min;
volatile float out_max;


void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){
        float i = *frame/32768.0;
        float o = detect( i );
        if( in_min  > i )in_min=i;
        if( in_max  < i )in_max=i;
        if( out_min > o )out_min=o;
        if( out_max < o )out_max=o;
    }
}





void audio_init(){  // HEADER

    assert( 0 == SDL_Init(SDL_INIT_AUDIO));
    atexit(SDL_Quit);
    
    SDL_AudioSpec want;
    SDL_zero(want);
    want.freq     = SAMPLE_RATE;
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






void audio_stats( float* _in_min, float* _in_max, float* _out_min, float* _out_max ){  // HEADER
    if(_in_min  ){ *_in_min  = in_min;  in_min  = 0; }
    if(_in_max  ){ *_in_max  = in_max;  in_max  = 0; }
    if(_out_min ){ *_out_min = out_min; out_min = 0; }
    if(_out_max ){ *_out_max = out_max; out_max = 0; }
}




