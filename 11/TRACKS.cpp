
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include "main.h"
#include <queue> // HEADER
#include <mutex> // HEADER







// HEADERBEG




struct TRACK {
    float min, max;
    bool enabled;
    int rgb[3];
    TRACK();
    void reset();
    void update( float in );
    void draw( int x, int y_midline, int h );
    void draw1( int x, int y_midline, int h );
};







struct TRACKS {
    TRACK I; // input
    TRACK O; // output
    TRACK R; // aux track red
    TRACK G; // aux track green
    TRACK C; // aux track cyan
    TRACKS();
    void reset();
};




struct TRACKS_MANAGER {
    TRACKS tracks;
    std::queue<TRACKS> fifo;
    std::mutex mutex;
    int msec;
    TRACKS_MANAGER();
    void reset();
    void store();
    TRACKS pop();
    // helpers
    TRACK& I = tracks.I;
    TRACK& O = tracks.O;
    TRACK& R = tracks.R;
    TRACK& G = tracks.G;
    TRACK& C = tracks.C;
};

// HEADEREND










#define RGB(VAR,R,G,B) { VAR[0]=R; VAR[1]=G; VAR[2]=B; }





//     _____ ____      _    ____ _  __
//    |_   _|  _ \    / \  / ___| |/ /
//      | | | |_) |  / _ \| |   | ' / 
//      | | |  _ <  / ___ \ |___| . \ 
//      |_| |_| \_\/_/   \_\____|_|\_\
//                                    


TRACK::TRACK(){
    enabled=false;
    RGB( rgb, 255, 64, 255 ); // my eyeees!
    reset();
}

void TRACK::reset(){
    min = +1E9;
    max = -1E9;
}

void TRACK::update( float in ){
    min = std::min(min,in);
    max = std::max(max,in);
    enabled = true;
}


void TRACK::draw( int x, int y_midline, int h ){
    // valori < 0 van sotto (y sdl grandi)
    // valori > 0 van sopra (y sdl piccoli)
    const float y0 = y_midline-max*h;
    const float y1 = y_midline-min*h;
    const SDL_Rect rect ={
        .x = x,
        .y = (int)y0,
        .w = 1,
        .h = (int)(y1-y0)+1,
    };
    SDL_SetRenderDrawColor( renderer, rgb[0],rgb[1],rgb[2], 255 );
    SDL_RenderFillRect( renderer, &rect );
}



void TRACK::draw1( int x, int y_midline, int h ){
    const float y0 = y_midline+h;
    const float y1 = y0-(max-min)*h;
    const SDL_Rect rect ={
        .x = x,
        .y = (int)y0,
        .w = 1,
        .h = (int)(y1-y0)+1,
    };
    SDL_SetRenderDrawColor( renderer, rgb[0],rgb[1],rgb[2], 255 );
    SDL_RenderFillRect( renderer, &rect );
}






//     _____ ____      _    ____ _  ______  
//    |_   _|  _ \    / \  / ___| |/ / ___| 
//      | | | |_) |  / _ \| |   | ' /\___ \ 
//      | | |  _ <  / ___ \ |___| . \ ___) |
//      |_| |_| \_\/_/   \_\____|_|\_\____/ 
//                                          


TRACKS::TRACKS(){
    RGB( I.rgb, 255, 255, 255 );
    RGB( O.rgb, 255, 255, 255 );
    RGB( R.rgb, 255, 128, 128 );
    RGB( G.rgb, 128, 255, 128 );
    RGB( C.rgb, 128, 255, 255 );
    reset();
}

void TRACKS::reset(){
    I.reset();
    O.reset();
    R.reset();
    G.reset();
    C.reset();
}











//     __  __    _    _   _    _    ____ _____ ____  
//    |  \/  |  / \  | \ | |  / \  / ___| ____|  _ \ 
//    | |\/| | / _ \ |  \| | / _ \| |  _|  _| | |_) |
//    | |  | |/ ___ \| |\  |/ ___ \ |_| | |___|  _ < 
//    |_|  |_/_/   \_\_| \_/_/   \_\____|_____|_| \_\
//                                                   

TRACKS_MANAGER::TRACKS_MANAGER(){
    msec = 10;
    reset();
}

void TRACKS_MANAGER::reset(){
    tracks.reset();
}

void TRACKS_MANAGER::store(){
    mutex.lock();
    fifo.push(tracks);
    mutex.unlock();
    reset();
}


TRACKS TRACKS_MANAGER::pop(){
    mutex.lock();
    auto Ts = fifo.front();
    fifo.pop();
    mutex.unlock();
    return Ts;
}




TRACKS_MANAGER tracks;  // HEADER





