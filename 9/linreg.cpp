
#undef NDEBUG
#include <assert.h>



void linreg( float* m, float* c, float x[], float y[], int n ){ // HEADER
    assert( x );
    assert( y );
    assert( n > 0 );

    float sumx  = 0;
    float sumy  = 0;
    float sumxx = 0;
    float sumxy = 0;

    for( int i=0; i<n; i++ ){
        sumx  += x[i];
        sumy  += y[i];
        sumxx += x[i]*x[i];
        sumxy += x[i]*y[i];
    }

    float d = n*sumxx-sumx*sumx;
    if(*m) *m = (n*sumxy-sumx*sumy)/d;
    if(*c) *c = (sumy*sumxx-sumx*sumxy)/d;
}



