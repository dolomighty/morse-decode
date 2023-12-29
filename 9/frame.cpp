
#undef NDEBUG
#include <assert.h>

#include <SDL2/SDL.h>   // HEADER
#include <SDL2/SDL_ttf.h>
#include "main.h"
//#include "kmeans.h"
#include "audio.h"
#include "COUNT.h"
#include "loop.h"
#include "detect.h"
#include "TTF_printf.h"





static void track_draw( float max, float min, int x, int y, int r, int g, int b ){
    const float y0 = y-max*(WINH*1/4);
    const float y1 = y-min*(WINH*1/4);
    const SDL_Rect rect ={
        .x = x,
        .y = (int)y0,
        .w = 1,
        .h = (int)(y1-y0)+1,
    };
    SDL_SetRenderDrawColor( renderer, r,g,b, 255 );
    SDL_RenderFillRect( renderer, &rect );
}







static void delta_draw(){

    static int max = -9999999;

    for( auto x: space_delta ) if(max<x)max=x;
    for( auto x: mark_delta  ) if(max<x)max=x;

    if(max<=0)return;

    SDL_SetRenderDrawColor( renderer, 0,255,255, 255 );
    for( auto x: mark_delta ){
        if(x<=0)continue;
        SDL_Rect r ={
            .x = x*WINW/max,
            .y = WINH-10,
            .w = 1,
            .h = 3,
        };
        SDL_RenderFillRect( renderer, &r );
    }

    SDL_SetRenderDrawColor( renderer, 255,0,0, 255 );
    for( auto x: space_delta ){
        if(x<=0)continue;
        SDL_Rect r ={
            .x = x*WINW/max,
            .y = WINH-5,
            .w = 1,
            .h = 3,
        };
        SDL_RenderFillRect( renderer, &r );
    }
}





static void vline_draw(){
    SDL_SetRenderDrawColor( renderer, 0,0,255, 255 );
    SDL_Rect r ={
        .x = mouse_x,
        .y = 0,
        .w = 1,
        .h = WINH,
    };
    SDL_RenderFillRect( renderer, &r );
}








void frame( SDL_Event event, float dt ){  // HEADER

    static float now;
    now += dt;


    static SDL_Texture *tex = 0;
    if(!tex){
        assert( tex = SDL_CreateTexture( renderer
            , SDL_PIXELFORMAT_RGB24
            , SDL_TEXTUREACCESS_TARGET
            , WINW, WINH ));
        SDL_SetRenderTarget( renderer, tex);
        SDL_SetRenderDrawColor( renderer, 0,0,0, 255 );
        SDL_RenderClear( renderer );
        SDL_SetRenderTarget( renderer, 0 );
    }








    if(audio_stats.size()>0){

        SDL_SetRenderTarget( renderer, tex );

        while(audio_stats.size()>0){
            auto stats = audio_stats.front();
            audio_stats.pop();

            static int x;
            x++;
            if(x>=WINW)x=0;

            // wiper
            {
                SDL_Rect r ={
                    .x = x,
                    .y = 0,
                    .w = 10,
                    .h = WINH,
                };
                SDL_SetRenderDrawColor( renderer, 0,0,0, 32 );
                SDL_RenderFillRect( renderer, &r );
                r.x += r.w;
                r.w *= 2;
                SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );
                SDL_RenderFillRect( renderer, &r );
                SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_NONE );
            }

            /************/            track_draw( stats.imax, stats.imin, x, WINH*1/4, 255,255,255 );
            /************/            track_draw( stats.omax, stats.omin, x, WINH*3/4, 255,255,255 );
            if(audio_aux_red_enabled) track_draw( stats.rmax, stats.rmin, x, WINH*3/4, 255,128,128 );
            if(audio_aux_grn_enabled) track_draw( stats.gmax, stats.gmin, x, WINH*3/4, 128,255,128 );

        }

        SDL_SetRenderTarget( renderer, 0 );
    }

    SDL_RenderCopy( renderer, tex, 0, 0 );



    if(audio_paused && fmod(now,1)>0.5){
        SDL_SetRenderDrawColor( renderer, 255,255,0, 32 );
        SDL_Rect r ={
            .x = 10,
            .y = 10,
            .w = 5,
            .h = 15,
        };
        SDL_RenderFillRect( renderer, &r );
        r.x = 20;
        SDL_RenderFillRect( renderer, &r );
    }

    vline_draw();
    delta_draw();
}



