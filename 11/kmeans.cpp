
#include <stdlib.h>
#include <string.h>




static void constrain( float means[], int nmeans, float samples[], int nsamples ){
    // controlla che le means non sian fuori dal range dei samples
    // quelle fuori range vengono reinizializzate
    float min = +1E9;
    float max = -1E9;
    for( int s=0; s<nsamples; s++ ){
        if(min>samples[s])min=samples[s];
        if(max<samples[s])max=samples[s];
    }

    for( int m=0; m<nmeans; m++ ){
        if(means[m]>min && means[m]<max)continue;
        // lerp
        means[m] = min+(max-min)*m/(nmeans-1);
    }
}




int kmeans_classify( float sample, float means[], int nmeans ){ // HEADER
    int color;
    float min_d = 1E9; // +inf
    for( int m=0; m<nmeans; m++ ){
        float d = sample - means[m];
        d *= d;
        if( min_d < d )continue;
        min_d = d;
        color = m;
    }
    return color;
}




void kmeans_step( float means[], int nmeans, float samples[], int nsamples ){ // HEADER
    // implementa uno step kmeans, aggiornando means con i nuovi dati

    // dividiamo i samples tra i gruppi
    // means deve già esser inizializzata, e verrà modificata
    int   colors[nsamples];
    float sum[nmeans]={0};
    int   cnt[nmeans]={0};
    for( int s=0; s<nsamples; s++ ){
        int color = kmeans_classify(samples[s],means,nmeans);
        colors[s]=color;
        // accumuliamo x il calcolo delle nuove means
        sum[color]+=samples[s];
        cnt[color]++;
    }

    // nuove means
    for( int color=0; color<nmeans; color++ ){
        if(cnt[color]<=0)continue;
        means[color] = sum[color] / cnt[color];
    }
}





#define MEMEQ(A,B,N) (0==memcmp(A,B,N))


void kmeans_update( float means[], int nmeans, float samples[], int nsamples ){ // HEADER
    // full update
    // fa n passi fino a che le means non cambiano da una gen alla successiva
    constrain( means, nmeans, samples, nsamples );
    for( int n=0; n<100; n++ ){
        float before[nmeans];
        memcpy( before, means, sizeof(before));
        kmeans_step( means, nmeans, samples, nsamples );
        if(MEMEQ( before, means, sizeof(before)))return; // sono uguali, abbiam finito
    }
}


