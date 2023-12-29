
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "fft.h"
#include "kmeans.h"
#include "scan_for_signal.h"
#include "show_spec.h"

// early crash via assert
#undef NDEBUG
#include <assert.h>



#include <string>
#include <unordered_map>

std::unordered_map<std::string, std::string> utf8_from_morse = {
    { ".-"     , "A" },
    { "-..."   , "B" },
    { "-.-."   , "C" },
    { "-.."    , "D" },
    { "."      , "E" },
    { "..-."   , "F" },
    { "--."    , "G" },
    { "...."   , "H" },
    { ".."     , "I" },
    { ".---"   , "J" },
    { "-.-"    , "K" },
    { ".-.."   , "L" },
    { "--"     , "M" },
    { "-."     , "N" },
    { "---"    , "O" },
    { ".--."   , "P" },
    { "--.-"   , "Q" },
    { ".-."    , "R" },
    { "..."    , "S" },
    { "-"      , "T" },
    { "..-"    , "U" },
    { "...-"   , "V" },
    { ".--"    , "W" },
    { "-..-"   , "X" },
    { "-.--"   , "Y" },
    { "--.."   , "Z" },
    { "-----"  , "0" },
    { ".----"  , "1" },
    { "..---"  , "2" },
    { "...--"  , "3" },
    { "....-"  , "4" },
    { "....."  , "5" },
    { "-...."  , "6" },
    { "--..."  , "7" },
    { "---.."  , "8" },
    { "----."  , "9" },
    { ".-.-.-" , "." },
    { "--..--" , "," },
    { "..--.." , "?" },
    { ".-.---" , "!" },
    { "-..-."  , "/" },
    { "-.--."  , "(" },
    { "-.--.-" , ")" },
};


#define FFT_LEN 256

SDL_AudioSpec have;


#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))  // HEADER




std::string symbol="";


void on_mark( int msecs ){
//    fprintf(stderr,"%4d msecs mark\n",len);

    // manteniamo una lista dei recenti marks
    // come base statistica da passare al kmeans
    // per distinguere tra punti e linee

    static float marks[20]={0};
    memmove( &marks[1], &marks[0], sizeof(marks[0])*(COUNT(marks)-1));
    marks[0]=msecs;
//    fprintf(stderr,"%.1f %.1f %.1f %.1f %.1f\n"
//        ,marks[0]
//        ,marks[1]
//        ,marks[2]
//        ,marks[3]
//        ,marks[4]
//    );

    // la statistica mostra 2 gruppi
    static float means[]={100,300};
    kmeans_update(means,COUNT(means),marks,COUNT(marks));
//    fprintf(stderr,"len %f ti %f ta %f \n"
//        ,(float)len
//        ,means[0]
//        ,means[1]
//    );

    int color = kmeans_classify(msecs,means,COUNT(means));
//    if(color==0) fprintf(stderr,".");
//    if(color==1) fprintf(stderr,"-");



    if(color==0) symbol+=".";
    if(color==1) symbol+="-";
}








void on_space( int msecs ){
//    fprintf(stderr,"%4d msecs space\n",len);

    // come per i marks
    // li passiamo al kmeans per distinguere tra spazi lunghi e brevi

    static float spaces[20]={0};
    memmove( &spaces[1], &spaces[0], sizeof(spaces[0])*(COUNT(spaces)-1));
    spaces[0]=msecs;
//    fprintf(stderr,"%.1f %.1f %.1f %.1f %.1f\n"
//        ,spaces[0]
//        ,spaces[1]
//        ,spaces[2]
//        ,spaces[3]
//        ,spaces[4]
//    );

    // la statistica mostra 4 gruppi
    static float means[]={100,300,600,1500};
    kmeans_update(means,COUNT(means),spaces,COUNT(spaces));
//    fprintf(stderr,"len %f ti %f ta %f \n"
//        ,(float)len
//        ,means[0]
//        ,means[1]
//        ,means[2]
//    );

    int color = kmeans_classify(msecs,means,COUNT(means));

    if(color>=1){
        // qualunque spazio che possa terminare un simbolo
        fprintf(stderr,"%s",utf8_from_morse[symbol].c_str());
        symbol="";
    }

//    if(color==0) fprintf(stderr," "); // spazio intersegnale
//    if(color==1) fprintf(stderr," "); // intersimbolo
    if(color==2) fprintf(stderr,"\n"); // interfrase
    if(color==3) fprintf(stderr,"\n\n\n"); // interdiscorso

}







void on_spec( float *spec, int len ){

//    show_spec( spec );

    // determiniamo se nello spettro c'è un segnale o no
    bool signal = scan_for_signal( spec, len );

    // edge trigger
    static bool signal_pre = false;
    static Uint32 msec_pre = 0;
    if( !signal_pre && signal ){
        // space → mark
        Uint32 msec = SDL_GetTicks();
//        printf("%d,%d,\n",msec,msec-msec_pre);
        on_space(msec-msec_pre);
        msec_pre = msec;
    }
    if( signal_pre && !signal ){
        // mark → space
        Uint32 msec = SDL_GetTicks();
//        printf("%d,,%d\n",msec,msec-msec_pre);
        on_mark(msec-msec_pre);
        msec_pre = msec;
    }
    signal_pre = signal;
}








void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){
        float *spec = fft_process(*frame/32768.0);
        if(spec) on_spec(spec,FFT_LEN/2);
    }
}




int main( int argc, char *argv[] ){ 

    fft_init(FFT_LEN);

    assert( 0 == SDL_Init(SDL_INIT_AUDIO));
    atexit(SDL_Quit);
    
    SDL_AudioSpec     want;
    SDL_AudioDeviceID dev;

    SDL_zero(want);
    want.freq     = 11025;
    want.format   = AUDIO_S16;
    want.channels = 1;
    want.samples  = 256;
    want.callback = MyAudioCallback;

    assert( dev = SDL_OpenAudioDevice(0,SDL_TRUE,&want,&have,0));
    
    SDL_PauseAudioDevice(dev,0);  // start audio activity
//    while(SDL_GetTicks()<10000)
    while(1)
    {
//        int a = rms_min*130;
//        int b = rms_max*130;
//        int i=0;
//        while( i++ < a )putchar('.');
//        while( i++ < b )putchar('=');
//        putchar('\n');

//        int a = (rms_min+rms_max)/2*130;
//        int i=0;
//        while( i++ < a )putchar('=');
//        putchar('\n');

//        if(signal)putchar('=');
//        putchar('\n');

//        show_spec();
//
//        fflush(stdout);
        SDL_Delay(200);
    }
    SDL_CloseAudioDevice(dev);

    return 0;
}


