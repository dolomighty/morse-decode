
#undef NDEBUG
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"


#define TTF_POINTS 14  // HEADER

static TTF_Font* font;



void TTF_printf( const int x, const int y, const char* fmt, ... ){ // HEADER

    char s[1024];
    va_list ap;
    va_start( ap, fmt );
    vsnprintf( s, sizeof(s), fmt, ap );
    va_end( ap );

    SDL_Color color = { 255,255,255 };

    SDL_Surface* ts;
    assert( ts = TTF_RenderUTF8_Blended( font, s, color ));

    SDL_Texture* tex;
    assert( tex = SDL_CreateTextureFromSurface( renderer, ts ));

    SDL_Rect r;
    r.x = x-ts->w/2;
    r.x = x;
    r.y = y;
    SDL_RenderCopy( renderer, tex, NULL, &r );
//    SDL_BlitSurface( ts, 0, renderer, &r );

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(ts);
};


void TTF_printf_init(){ // HEADER
    assert( 0 == TTF_Init());
    assert( font = TTF_OpenFont( "font.ttf", TTF_POINTS ));
//    font = TTF_OpenFont( "font.ttf", TTF_POINTS );
//    if(!font){
//        fprintf(stderr,"TTF_GetError: %s\n", TTF_GetError());
//        assert("TTF_OpenFont"==0);
//    }
}




/*


quick tut:

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

const int TTF_POINTS = 10;
TTF_Init();
TTF_Font* font;
assert( font = TTF_OpenFont( "font.ttf", TTF_POINTS ));
TTF_printf( 100, TTF_POINTS*4, "%.2f", fft_frames );
TTF_Quit();


*/
