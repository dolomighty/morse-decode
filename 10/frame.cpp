
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
#include "TRACKS.h"








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





static float lerp ( float a, float b, float t ){
    return a+(b-a)*t;
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








    if(tracks.fifo.size()>0){
//        fprintf(stderr,"tracks.fifo.size() %ld\n",tracks.fifo.size());

        SDL_SetRenderTarget( renderer, tex );

        while(tracks.fifo.size()>0){
            auto tr = tracks.pop();

            static int x;
            x++;
            if(x>=WINW)x=0;

            // wiper
            {
                SDL_Rect r ={
                    .x = x,
                    .y = 0,
                    .w = 20,
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

            // reticolo
            SDL_SetRenderDrawColor( renderer, 64,64,64, 255 );
            {
                SDL_Rect r ={
                    .x = x,
                    .w = 1,
                    .h = 1,
                };
                r.y = WINH*4/8; SDL_RenderFillRect( renderer, &r );
                r.y = WINH*5/8; SDL_RenderFillRect( renderer, &r );
                r.y = WINH*6/8; SDL_RenderFillRect( renderer, &r );
                r.y = WINH*7/8; SDL_RenderFillRect( renderer, &r );
                r.y = WINH*8/8; SDL_RenderFillRect( renderer, &r );
            }

            tr.I.draw( x, WINH*1/4, WINH*1/4 );
            tr.O.draw( x, WINH*3/4, WINH*1/4 );
            tr.R.draw( x, WINH*3/4, WINH*1/4 );
            tr.G.draw( x, WINH*3/4, WINH*1/4 );
            tr.C.draw( x, WINH*3/4, WINH*1/4 );

            // epsilon
            SDL_SetRenderDrawColor( renderer, 255,0,255, 255 );
            {
                SDL_Rect r ={
                    .x = x,
                    .y = (int)((WINH*3/4)-detect_eps*(WINH*1/4)*y_scale),
                    .w = 1,
                    .h = 1,
                };
                SDL_RenderFillRect( renderer, &r );
            }
        }

        SDL_SetRenderTarget( renderer, 0 );
    }

    SDL_RenderCopy( renderer, tex, 0, 0 );



    if(audio_paused || !audio_running){
        if(fmod(now,1)>0.5){
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
    }else {
        // TODO: disegnare una serie di >>>
        // per indicare la velocità di tracciamento
    }

    vline_draw();
    delta_draw();

//    TTF_printf( 10, 10, "%d msec", tracks.msec );
}



