
#undef NDEBUG
#include <assert.h>

#include <SDL2/SDL.h>   // HEADER
#include "main.h"
//#include "kmeans.h"
#include "audio.h"
#include "COUNT.h"





void frame( SDL_Event event, float dt ){  // HEADER


    static int x;
    x++;
    if(x>=WINW)x=0;




    static SDL_Texture *tex = 0;
    if(!tex){
        assert( tex = SDL_CreateTexture( renderer
            , SDL_PIXELFORMAT_RGB24
            , SDL_TEXTUREACCESS_TARGET
            , WINW, WINH ));
//        SDL_SetTextureBlendMode( tex, SDL_BLENDMODE_BLEND );
        SDL_SetRenderTarget( renderer, tex);
        SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
        SDL_RenderClear( renderer );
        SDL_SetRenderTarget( renderer, 0 );
    }






    SDL_SetRenderTarget( renderer, tex );

    float in_min;
    float in_max;
    float out_min;
    float out_max;
    audio_stats( &in_min, &in_max, &out_min, &out_max );

    {
        const SDL_Rect r ={
            .x = x,
            .y = 0,
            .w = 50,
            .h = WINH,
        };
        SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
        SDL_RenderFillRect( renderer, &r );
    }

    {
        const float y0 = (WINH*1/4)-in_max*(WINH*1/4);
        const float y1 = (WINH*1/4)-in_min*(WINH*1/4);
        const SDL_Rect r ={
            .x = x,
            .y = (int)y0,
            .w = 1,
            .h = (int)(y1-y0)+1,
        };
        SDL_SetRenderDrawColor( renderer, 255,255,255, 255 );
        SDL_RenderFillRect( renderer, &r );
    }

    {
        const float y0 = (WINH*3/4)-out_max*(WINH*1/4);
        const float y1 = (WINH*3/4)-out_min*(WINH*1/4);
        const SDL_Rect r ={
            .x = x,
            .y = (int)y0,
            .w = 1,
            .h = (int)(y1-y0)+1,
        };
        SDL_SetRenderDrawColor( renderer, 255,128,128, 255 );
        SDL_RenderFillRect( renderer, &r );
    }

    SDL_SetRenderTarget( renderer, 0 );

    SDL_RenderCopy( renderer, tex, 0, 0 );
}


