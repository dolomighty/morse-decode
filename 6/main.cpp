
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "kmeans.h"
#include "signal_detect.h"
#include "acorr.h"
#include "lags_print.h"
#include <termios.h>

// early crash via assert
#undef NDEBUG
#include <assert.h>

#include <string>
#include <unordered_map>



std::unordered_map<std::string, std::string> utf8_from_morse = {
    { ".-"    , "A" },
    { "-..."  , "B" },
    { "-.-."  , "C" },
    { "-.."   , "D" },
    { "."     , "E" },
    { "..-."  , "F" },
    { "--."   , "G" },
    { "...."  , "H" },
    { ".."    , "I" },
    { ".---"  , "J" },
    { "-.-"   , "K" },
    { ".-.."  , "L" },
    { "--"    , "M" },
    { "-."    , "N" },
    { "---"   , "O" },
    { ".--."  , "P" },
    { "--.-"  , "Q" },
    { ".-."   , "R" },
    { "..."   , "S" },
    { "-"     , "T" },
    { "..-"   , "U" },
    { "...-"  , "V" },
    { ".--"   , "W" },
    { "-..-"  , "X" },
    { "-.--"  , "Y" },
    { "--.."  , "Z" },
    { "-----" , "0" },
    { ".----" , "1" },
    { "..---" , "2" },
    { "...--" , "3" },
    { "....-" , "4" },
    { "....." , "5" },
    { "-...." , "6" },
    { "--..." , "7" },
    { "---.." , "8" },
    { "----." , "9" },
    { ".-.-.-", "." },
    { "--..--", "," },
    { "..--..", "?" },
    { ".-.---", "!" },
    { "-..-." , "/" },
    { "-.--." , "(" },
    { "-.--.-", ")" },
};


const int SAMPLE_RATE = 11025;

SDL_AudioSpec have;


#define COUNT(ARR) (sizeof(ARR)/sizeof(ARR[0]))  // HEADER




std::string symbol="";

float thresh = 2;




const char LAGS  = 'L';
const char STATS = 'S';
const char XLATE = 'T';
const char MORSE = 'M';
char mode = LAGS;





struct {
    float space[4]; // la statistica mostra 4 gruppi
    float mark[2];  // la statistica mostra 2 gruppi
} means;


void stats_print(){
    fprintf(stderr,"\rspace %.2f %.2f %.2f %.2f mark %.2f %.2f  "
        ,means.space[0]
        ,means.space[1]
        ,means.space[2]
        ,means.space[3]
        ,means.mark[0]
        ,means.mark[1]
    );
}



void on_mark( int frame ){
//    fprintf(stderr,"%4d frame mark\n",len);

    // manteniamo una lista dei recenti marks
    // come base statistica da passare al kmeans
    // per distinguere tra punti e linee
    static float marks[20]={0};
    memmove( &marks[1], &marks[0], sizeof(marks[0])*(COUNT(marks)-1));
    marks[0]=frame;

    // la statistica mostra due gruppi
    // COUNT(means.mark) = 2
    kmeans_update( means.mark, COUNT(means.mark), marks, COUNT(marks));

    if(mode == STATS) stats_print();

    int color = kmeans_classify( frame, means.mark, COUNT(means.mark));

    if(mode==MORSE){
        if(color==0) fprintf(stderr,".");
        if(color==1) fprintf(stderr,"-");
    }

    if(color==0) symbol+=".";
    if(color==1) symbol+="-";
}








void on_space( int frame ){
//    fprintf(stderr,"%4d frame space\n",len);

    // come per i marks
    // li passiamo al kmeans per distinguere tra spazi lunghi e brevi
    static float spaces[10]={0};
    memmove( &spaces[1], &spaces[0], sizeof(spaces[0])*(COUNT(spaces)-1));
    spaces[0]=frame;

    // la statistica mostra 4 gruppi
    kmeans_update( means.space, COUNT(means.space), spaces, COUNT(spaces));

    if(mode == STATS) stats_print();

    int color = kmeans_classify( frame, means.space, COUNT(means.space));
//    if(color==0) fprintf(stderr," "); // spazio intersegnale - debug

    if(mode == XLATE){
        if(color>=1){
            // qualunque spazio che possa terminare un simbolo
            fprintf(stderr,"%s",utf8_from_morse[symbol].c_str());
            symbol="";
        }
        if(color==2) fprintf(stderr,"\n"); // interfrase
        if(color==3) fprintf(stderr,"\n\n\n"); // interdiscorso
    }

    if(mode == MORSE){
        if(color==1) fprintf(stderr," "); // intersimbolo
        if(color==2) fprintf(stderr,"\n"); // interfrase
        if(color==3) fprintf(stderr,"\n\n\n"); // interdiscorso
    }
}







void on_lags( float *lags, int n ){

    static int frame;
    frame++;

    // determiniamo se nello spettro c'è un segnale o no
    bool signal = signal_detect( lags, n, thresh );

    // edge trigger
    static bool signal_pre = false;
    static int frame_pre = 0;

    if( !signal_pre && signal ){
        // space → mark
//        printf("%d,%d,\n",frame,frame-frame_pre);
        on_space(frame-frame_pre);
        frame_pre = frame;
    }

    if( signal_pre && !signal ){
        // mark → space
//        printf("%d,,%d\n",frame,frame-frame_pre);
        on_mark(frame-frame_pre);
        frame_pre = frame;
    }

    signal_pre = signal;

    if( mode == LAGS ) lags_print( lags, n );
}





void MyAudioCallback( void *userdata, Uint8 *stream, int len_bytes ){
    short *frame = (short*)stream;
    int len_frames = len_bytes / sizeof(*frame);
    for (; len_frames > 0; len_frames--, frame++ ){
        float v = *frame/32768.0;

//        static float lp = 0;
//        lp = lp+(v-lp)*0.5;

        float *lags = acorr_in(v,50);
        if(lags) on_lags(lags,50);
    }
}








void stdin_no_buffering_loop(){
    
    int stdin_fd = 0;
//    fd_set fds;
    
    // magia per disattivare il buffering sul terminale
    struct termios oldSettings, newSettings;
    tcgetattr( stdin_fd, &oldSettings );
    newSettings = oldSettings;
    newSettings.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr( stdin_fd, TCSANOW, &newSettings );
    
    while(1){
//        FD_ZERO ( &fds );
//        FD_SET ( stdin_fd, &fds );
//
//        select ( FD_SETSIZE, &fds, 0, 0, 0 );
//
//        if ( FD_ISSET ( stdin_fd, &fds )) {
//            char    c;
//            if ( read ( stdin_fd, &c, 1 ) <= 0 ) break;
//            switch ( c ) {
//                case ' ' : 
//                    mode = 0 ;
//                    printf ( "performance\n" );
//                    break;
//                case '+' :
//                    mode = 1 ;
//                    printf ( "increment\n" );
//                    break;
//                case '-' :
//                    mode = 2 ;
//                    printf ( "decrement\n" );
//                    break;
//            }
//        }

        while(1){
            switch(getchar()){
                case 'q': goto done;
                case 'l' : mode=LAGS;  fprintf(stderr, "\nmode %c\n" ,mode); break;
                case 's' : mode=STATS; fprintf(stderr, "\nmode %c\n" ,mode); break;
                case 't' : mode=XLATE; fprintf(stderr, "\nmode %c\n" ,mode); break;
                case 'm' : mode=MORSE; fprintf(stderr, "\nmode %c\n" ,mode); break;
                case '+' : thresh+=0.1; fprintf(stderr, "\nthresh %.2f\n" ,thresh); break;
                case '-' : thresh-=0.1; fprintf(stderr, "\nthresh %.2f\n" ,thresh); break;
            }
            
        }
        
    }
done:
    tcsetattr( stdin_fd, TCSANOW, &oldSettings );
}







int main( int argc, char *argv[] ){ 

    fprintf(stderr,"init...\n");

    assert( 0 == SDL_Init(SDL_INIT_AUDIO));
    atexit(SDL_Quit);
    
    SDL_AudioSpec     want;
    SDL_AudioDeviceID dev;

    SDL_zero(want);
    want.freq     = SAMPLE_RATE;
    want.format   = AUDIO_S16;
    want.channels = 1;
    want.samples  = 256;
    want.callback = MyAudioCallback;

    assert( dev = SDL_OpenAudioDevice(0,SDL_TRUE,&want,&have,0));
    
    SDL_PauseAudioDevice(dev,0);  // start audio activity
//    while(SDL_GetTicks()<10000)

    fprintf(stderr,"running\n");

    stdin_no_buffering_loop();

    SDL_CloseAudioDevice(dev);

    return 0;
}






