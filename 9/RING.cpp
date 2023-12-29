
#include <stdlib.h>
#include <string.h>

// early crash via assert
#undef NDEBUG
#include <assert.h>


// HEADERBEG
class RING {
    private:
        float *buffer;
        int head;
        int len;
    public:
        void init( int _len );
        float wr( float in );
        float* snap( float *samples, int n );
};
// HEADEREND


    
void RING::init( int _len ){
    assert( _len > 0 );
    len = _len;
    assert( buffer = (float*)calloc( len*2, sizeof(buffer[0])));
}



float RING::wr( float in ){
    assert( buffer );
    assert( len );
    buffer[head]     = in;
    buffer[head+len] = in;
    head = (head+1)%len;
    //      head ↓
    // ...1234567.....1234567.. buffer
    // |--fftlen--||--fftlen--|
    return in;
}



float* RING::snap( float *samples, int n ){
    assert( buffer );
    assert( len );
    assert( samples );
    assert( n > 0 );
    memcpy( samples, &buffer[head+len-n], n*sizeof(samples[0]));
    //                   ↓ head+ring.len-n
    //      head ↓           ↓ head+ring.len
    // ...1234567.....1234567.. buffer
    // |--fftlen--||--fftlen--|
    return samples;
}


