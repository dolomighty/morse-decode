
#include <SDL2/SDL.h>
#include <stdint.h>
#include <time.h>
#include "frame.h"
#include "main.h"
#include "detect.h"
#include "audio.h"
#include "TRACKS.h"



void change_tracks_msec( int msec ){
    tracks.msec = msec;
    fprintf( stderr, "tracks.msec %d\n", tracks.msec );
}


void change_detect_eps( float delta ){
    detect_eps += delta;
//    if(detect_eps<0)detect_eps=0;
    fprintf( stderr, "detect_eps %f\n", detect_eps );
}


void change_y_scale( float delta ){
    y_scale += delta*(1+y_scale)*10;
    if(y_scale<1)y_scale=1;
    static float pre_y_scale;
    if(pre_y_scale==y_scale)return;
    pre_y_scale=y_scale;
    fprintf( stderr, "y_scale %f\n", y_scale );
}


int mouse_x; // HEADER





void loop(){    // HEADER

    static bool key_detect_eps;
    static bool key_y_scale;
    


    while(1){
        
        SDL_Event event;
        while( SDL_PollEvent(&event)){
            switch( event.type ){
                
                case SDL_MOUSEWHEEL:
//                    fprintf( stderr, "SDL_MOUSEWHEEL %d,%d\n"
//                        , event.wheel.x
//                        , event.wheel.y
//                    );
                    if(key_detect_eps) change_detect_eps ( event.wheel.y*1E-3 );
                    if(key_y_scale)    change_y_scale    ( event.wheel.y*1E-3 );
                    break;

                case SDL_KEYDOWN:
//                    fprintf( stderr, "SDL_KEYDOWN %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    switch( event.key.keysym.sym ){
                        case SDLK_1: change_tracks_msec(1);  break;
                        case SDLK_2: change_tracks_msec(2);  break;
                        case SDLK_3: change_tracks_msec(5);  break;
                        case SDLK_4: change_tracks_msec(10); break;
                        case SDLK_5: change_tracks_msec(20); break;
                        case SDLK_e: key_detect_eps = true; SDL_SetRelativeMouseMode(SDL_TRUE); break;
                        case SDLK_y: key_y_scale    = true; SDL_SetRelativeMouseMode(SDL_TRUE); break;
                        case SDLK_SPACE: audio_paused = !audio_paused; break;
                    }
                    break;

                case SDL_KEYUP:
//                    fprintf( stderr, "SDL_KEYUP %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    key_detect_eps = false;
                    key_y_scale    = false;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;

                case SDL_MOUSEMOTION:
//                    fprintf( stderr, "SDL_MOUSEMOTION %d,%d abs %d,%d\n"
//                        ,event.motion.xrel
//                        ,event.motion.yrel
//                        ,event.motion.x
//                        ,event.motion.y
//                    );
                    mouse_x = event.motion.x;
                    break;

//                case SDL_MOUSEBUTTONDOWN:
//                    fprintf( stderr, "SDL_MOUSEBUTTONDOWN %d @ %d,%d\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y
//                    );
//                    switch( event.button.button ){
//                        case SDL_BUTTON_MIDDLE: fprintf( stderr, "SDL_BUTTON_MIDDLE\n" ); break;
//                        case SDL_BUTTON_LEFT:   fprintf( stderr, "SDL_BUTTON_LEFT\n"   ); break;
//                        case SDL_BUTTON_RIGHT:  fprintf( stderr, "SDL_BUTTON_RIGHT\n"  ); break;
//                    }
//                    break;
//
//                case SDL_MOUSEBUTTONUP:
//                    fprintf( stderr, "SDL_MOUSEBUTTONUP %d @ %d,%d\n",
//                        event.button.button, 
//                        event.button.x, 
//                        event.button.y
//                    );
//                    switch( event.button.button ){
//                        case SDL_BUTTON_MIDDLE: fprintf( stderr, "SDL_BUTTON_MIDDLE\n" ); break;
//                        case SDL_BUTTON_LEFT:   fprintf( stderr, "SDL_BUTTON_LEFT\n"   ); break;
//                        case SDL_BUTTON_RIGHT:  fprintf( stderr, "SDL_BUTTON_RIGHT\n"  ); break;
//                    }
//                    break;

                case SDL_QUIT:
//                    fprintf( stderr, "SDL_QUIT\n" );
                    return;
            }
        }

        static int frame_count;

        static timespec beg;
        timespec end;
        clock_gettime(CLOCK_REALTIME, &end);
        float dt = (end.tv_sec-beg.tv_sec)+(end.tv_nsec-beg.tv_nsec)*1E-9;
        beg = end;

        if(frame_count==0) dt=0.1;

        frame_count++;
        frame( event, dt );

        SDL_RenderPresent(renderer);
//        SDL_Delay(1); // commentare in caso di SDL_RENDERER_PRESENTVSYNC
    }
}    




