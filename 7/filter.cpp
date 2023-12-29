
#include <math.h>
#include <stdlib.h>

// http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt

typedef struct BIQUAD {
    float x1, x2;
    float y1, y2;
    float b0_a0;
    float b1_a0;
    float b2_a0;
    float a1_a0;
    float a2_a0;
} BIQUAD;


void *filter_create () { // HEADER
    return malloc ( sizeof ( BIQUAD ));
}




void filter_bp_set ( void *filter, float omega, float Q ) { // HEADER
    BIQUAD * biquad = filter;
    
//  float omega = 2*M_PI * freq / RATE;
    float alpha = sin(omega)/(2*Q);
        
    float b0 =  alpha;
    float b1 = 0;
    float b2 = -alpha;
    float a0 =  1 + alpha;
    float a1 = -2*cos(omega);
    float a2 =  1 - alpha;
    
    biquad->b0_a0 = b0/a0;
    biquad->b1_a0 = b1/a0;
    biquad->b2_a0 = b2/a0;
    biquad->a1_a0 = a1/a0;
    biquad->a2_a0 = a2/a0;
}


void filter_lp_set ( void *filter, float omega, float Q ) { // HEADER
    BIQUAD * biquad = filter;
    
    float alpha = sin(omega)/(2*Q);
    
    float b0 =  (1 - cos(omega))/2;
    float b1 =   1 - cos(omega);
    float b2 =  (1 - cos(omega))/2;
    float a0 =   1 + alpha;
    float a1 =  -2*cos(omega);
    float a2 =   1 - alpha;

    biquad->b0_a0 = b0/a0;
    biquad->b1_a0 = b1/a0;
    biquad->b2_a0 = b2/a0;
    biquad->a1_a0 = a1/a0;
    biquad->a2_a0 = a2/a0;
}



float filter_process ( void *filter, float in ) { // HEADER
    BIQUAD * biquad = filter;

    float y = biquad->b0_a0 * in 
        + biquad->b1_a0 * biquad->x1 + biquad->b2_a0 * biquad->x2
        - biquad->a1_a0 * biquad->y1 - biquad->a2_a0 * biquad->y2;
    
    biquad->x2 = biquad->x1;
    biquad->x1 = in;
    biquad->y2 = biquad->y1;
    biquad->y1 = y;
    
    return y;
}



