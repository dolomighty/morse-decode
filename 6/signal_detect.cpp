
#include <stdio.h>
#include "kmeans.h"
#include "main.h"





bool signal_detect( float *lags, int n, float thresh ){       // HEADER
    // lavora sull'autocorrelazione via ASDF
    // quindi cerca le valli intorno a 0
    // saltiamo i primi lags perche
    // per definizione sono sempre autocorrelati
    for( int i=2; i<n; i++ ){
        if(lags[i]<thresh)return true;
    }
    return false;
}
