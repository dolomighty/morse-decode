
#include <SDL2/SDL.h>
#include "main.h"
#include "XY.h"
#include "COUNT.h"




XY means[3];    // i gruppi
XY samples[500];
int colors[COUNT(samples)];





static int find_color( XY sample ){
    // usa le medie attuali per colorare il sample
    float min_dd = 1E6; // +inf
    int color; // un sample è sicuramente di un colore, non serve init
    for( int m=0; m<COUNT(means); m++ ){
        XY d = sample-means[m];
        float dd = sqrt(d*d);
        if( min_dd < dd )continue;
        min_dd = dd;
        color = m;
    }
    return color;
}



static float lerp( float a, float b, float x ){
    return a+(b-a)*x;
}


//static XY random_in_circle( XY center, float radius ){
//    float r = radius * sqrt(rand()*1.0/RAND_MAX);
//    float theta = rand()*1.0/RAND_MAX * 2 * 3.141562;
//    return center+XY(cos(theta),sin(theta))*r;
//}




void kmeans_init(){ // HEADER
    
//    means[0] = XY( 
//        lerp( WINW*0.0/2, WINW*1.0/2, rand()*1.0/RAND_MAX ),
//        lerp( WINH*0.0/2, WINH*2.0/2, rand()*1.0/RAND_MAX )
//    );
//
//    means[1] = XY( 
//        lerp( WINW*1.0/2, WINW*2.0/2, rand()*1.0/RAND_MAX ),
//        lerp( WINH*0.0/2, WINH*2.0/2, rand()*1.0/RAND_MAX )
//    );

    for( int s=0; s<COUNT(samples); s++ ){

        auto xy = XY(
            lerp( WINW*0.0/2, WINW*2.0/2, rand()*1.0/RAND_MAX ),
            lerp( WINH*0.0/2, WINH*2.0/2, rand()*1.0/RAND_MAX )
        );

        samples[s] = xy;
        int color = find_color(xy);
        colors[s] = color;
    }
}





void kmeans_update(){ // HEADER    

    struct {
        XY accu;
        int n;
    } tmp_means[COUNT(means)];

    bzero(tmp_means,sizeof(tmp_means));

    for( int s=0; s<COUNT(samples); s++ ){
        int color = find_color(samples[s]);
        colors[s]=color;
        tmp_means[color].n++;
        tmp_means[color].accu = tmp_means[color].accu + samples[s];
    }

    for( int color=0; color<COUNT(means); color++ ){
        if(tmp_means[color].n<=0)continue;
        means[color] = tmp_means[color].accu / tmp_means[color].n;
    }
}



static void pix( XY xy ){
    SDL_Rect r ={
        .x = (int)(xy.x-1),
        .y = (int)(xy.y-1),
        .w = 3,
        .h = 3,
    };
    SDL_RenderFillRect( renderer, &r );
}


void kmeans_render(){ // HEADER
    for( int s=0; s<COUNT(samples); s++ ){
        if(colors[s]==0) SDL_SetRenderDrawColor( renderer, 255,0,0, 255 );
        if(colors[s]==1) SDL_SetRenderDrawColor( renderer, 0,255,0, 255 );
        if(colors[s]==2) SDL_SetRenderDrawColor( renderer, 0,0,255, 255 );
        pix( samples[s] );
    }
    SDL_SetRenderDrawColor( renderer, 255,255,255, 255 );
    for( int m=0; m<COUNT(means); m++ ){
        pix( means[m] );
    }
}



