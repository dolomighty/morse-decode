
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "COUNT.h"
#include "main.h"
#include "detect.h"
#include "fft.h"
#include "TRACKS.h"
#include <queue> // HEADER


#define FFT_LEN   256
#define AUDIO_FPS 22050 // HEADER

#define FRAMES_FROM_MSEC(MSEC)   ((MSEC)*AUDIO_FPS/1000) // HEADER
#define MSEC_FROM_FRAMES(FRAMES) ((FRAMES)*1000/AUDIO_FPS)   // HEADER





static SDL_AudioSpec     have;
static SDL_AudioDeviceID dev;


bool audio_paused  = false; // HEADER
bool audio_running = false; // HEADER




float softknee( float in ){   // HEADER
    // un semplice limiter:
    // 0  → 0
    // ±1 → ±0.5
    // ±∞ → ±1
    // OKKIO: va proprio usato in>=0
    // con in>0 va in segfault, credo parta una ricorsione infinita
    // per in=-0, che è sia negativo che =0... magie dello IEEE
    // invece con in>=0 viene agganciato il -0 come fosse =0 e va tutto bene
    if(in>=0) return in/(1+in);
    return -softknee(-in);
}








void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){

    // c'è un leggero ritardo tra lo start e l'effettiva prima chiamata
    // della callback. la status query non lo rileva, quindi vado di flag
    audio_running = true;

    if(audio_paused)return;
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){

        float i = *frame/32768.0;
        tracks.I.update(i);

        i = softknee( i*100 );

        float* fft = fft_in(i);
        if(fft) on_fft( fft, FFT_LEN/2 );

        static int timeout;
        timeout--;
        if(timeout>0)continue;
        timeout = FRAMES_FROM_MSEC(tracks.msec);

        tracks.store();
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

//    fft_init( FFT_LEN, FRAMES_FROM_MSEC( 1 ));
    fft_init( FFT_LEN, 1 ); // una fft ogni frame

    audio_running = false;
}






void audio_start(){  // HEADER
    SDL_PauseAudioDevice(dev,0);  // start audio activity
}







void audio_stop(){  // HEADER
    SDL_CloseAudioDevice(dev);
}


