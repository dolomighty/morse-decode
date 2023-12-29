
#include <stdlib.h>
#include <string.h>

// early crash via assert
#undef NDEBUG
#include <assert.h>




static struct RING {
    float *buffer;
    int head;
    int len;
} ring;



void ring_init( int len ){  // HEADER
    assert( ring.buffer = (float*)malloc( len * 2 * sizeof(ring.buffer[0])));
    ring.head = 0;
    ring.len = len;
}

    

void ring_wr( float sample ){  // HEADER
    ring.buffer[ring.head]         = sample;
    ring.buffer[ring.head+ring.len] = sample;
    ring.head = (ring.head+1)%ring.len;
    //      head ↓
    // ...1234567.....1234567.. buffer
    // |--fftlen--||--fftlen--|
}



void ring_snap( float *samples, int n ){  // HEADER
    memcpy( samples, &ring.buffer[ring.head+ring.len-n], n*sizeof(samples[0]));
    //                   ↓ head+ring.len-n
    //      head ↓           ↓ head+ring.len
    // ...1234567.....1234567.. buffer
    // |--fftlen--||--fftlen--|
}


