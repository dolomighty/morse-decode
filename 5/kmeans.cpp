
#include <stdlib.h>
#include <string.h>




static void constrain( float means[], int means_len, float samples[], int samples_len ){
    // controlla che le means non sian fuori dal range dei samples
    // quelle fuori range vengono reinizializzate
    float min = +1E9;
    float max = -1E9;
    for( int s=0; s<samples_len; s++ ){
        if(min>samples[s])min=samples[s];
        if(max<samples[s])max=samples[s];
    }

    for( int m=0; m<means_len; m++ ){
        if(means[m]>min && means[m]<max)continue;
        // lerp
        means[m] = min+(max-min)*m/(means_len-1);
    }
}





int kmeans_classify( float sample, float means[], int means_len ){ // HEADER
    int color;
    float min_d = 1E9; // +inf
    for( int m=0; m<means_len; m++ ){
        float d = sample - means[m];
        d *= d;
        if( min_d < d )continue;
        min_d = d;
        color = m;
    }
    return color;
}


void kmeans_step( float means[], int means_len, float samples[], int samples_len ){ // HEADER
    // implementa uno step kmeans, aggiornando means con i nuovi dati

    // dividiamo i samples tra i gruppi
    // means deve già esser inizializzata, e verrà modificata
    int   colors[samples_len];
    float sum[means_len]={0};
    int   cnt[means_len]={0};
    for( int s=0; s<samples_len; s++ ){
        int color = kmeans_classify(samples[s],means,means_len);
        colors[s]=color;
        // accumuliamo x il calcolo delle nuove means
        sum[color]+=samples[s];
        cnt[color]++;
    }

    // nuove means
    for( int color=0; color<means_len; color++ ){
        if(cnt[color]<=0)continue;
        means[color] = sum[color] / cnt[color];
    }
}


void kmeans_update( float means[], int means_len, float samples[], int samples_len ){ // HEADER
    // full update
    // fa n passi fino a che le means non cambiano da una gen alla successiva
    constrain( means, means_len, samples, samples_len );
    for( int n=0; n<100; n++ ){
        float before[means_len];
        memcpy( before, means, sizeof(before));
        kmeans_step( means, means_len, samples, samples_len );
        if( 0 == memcmp( before, means, sizeof(before)))return; // sono uguali, abbiam finito
    }
}



