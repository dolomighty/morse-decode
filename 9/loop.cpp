
#include <SDL2/SDL.h>
#include <stdint.h>
#include <time.h>
#include "frame.h"
#include "main.h"
#include "detect.h"
#include "audio.h"



void change_audio_stats_msec( int msec ){
    audio_stats_msec = msec;
    fprintf( stderr, "audio_stats_msec %d\n", audio_stats_msec );
}


void change_detect_eps( float delta ){
    detect_eps += delta;
    if(detect_eps<0)detect_eps=0;
    fprintf( stderr, "detect_eps %f\n", detect_eps );
}


int mouse_x; // HEADER





void loop(){    // HEADER

    static bool key_detect_eps;
    
    while(1){
        
        SDL_Event event;
        while( SDL_PollEvent(&event)){
            switch( event.type ){
                
//                case SDL_MOUSEWHEEL:
//                    fprintf( stderr, "SDL_MOUSEWHEEL %d,%d\n"
//                        , event.wheel.x
//                        , event.wheel.y
//                    );
//                    break;

                case SDL_KEYDOWN:
//                    fprintf( stderr, "SDL_KEYDOWN %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    switch( event.key.keysym.sym ){
                        case SDLK_1: change_audio_stats_msec(2);  break;
                        case SDLK_2: change_audio_stats_msec(5);  break;
                        case SDLK_3: change_audio_stats_msec(10); break;
                        case SDLK_4: change_audio_stats_msec(20); break;
                        case SDLK_e: key_detect_eps = true; break;
                        case SDLK_SPACE: audio_paused = !audio_paused; break;
                    }
                    break;

                case SDL_KEYUP:
//                    fprintf( stderr, "SDL_KEYUP %s\n", SDL_GetKeyName(event.key.keysym.sym));
                    key_detect_eps = false;
                    break;

                case SDL_MOUSEMOTION:
//                    fprintf( stderr, "SDL_MOUSEMOTION %d,%d abs %d,%d\n"
//                        ,event.motion.xrel
//                        ,event.motion.yrel
//                        ,event.motion.x
//                        ,event.motion.y
//                    );
                    mouse_x = event.motion.x;
                    if(key_detect_eps) change_detect_eps( -event.motion.yrel*1E-4 );
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




