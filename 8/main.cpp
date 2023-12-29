
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>   // HEADER
#include "loop.h"
#include "kmeans.h"
#include "audio.h"
#include "COUNT.h"




#define WINW 600    // HEADER
#define WINH 300    // HEADER




SDL_Renderer *renderer;     // HEADER



int main( int argc, char *argv[] ){ 

    char *basename = strrchr(argv[0],'/')+1;

    audio_init();

    SDL_Window *win;
    assert( win = 
        SDL_CreateWindow( basename, 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                WINW, WINH, 0 ));

    assert( renderer = SDL_CreateRenderer( win, -1, 
        SDL_RENDERER_PRESENTVSYNC | 
        SDL_RENDERER_ACCELERATED ));

    audio_start();

    loop();

    audio_stop();
    
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( win );
    
    return 0;
}


