
#undef NDEBUG
#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "COUNT.h"
#include "audio.h"
#include "linreg.h"
#include "DELAY.h"
#include "TRACKS.h"
#include <vector>  // HEADER
#include <deque>





struct STATS {
    double min;
    double max;
    double avg;
    double std;
};





STATS mk_stats( double in, int hlen ){

    static std::vector<double> hist;
    if(hist.size()!=hlen) hist.resize(hlen);

    static int head = 0;
    hist[head%hist.size()]=in;
    head++;

    STATS s = {};
    s.min = hist[0];
    s.max = hist[0];
    s.avg = 0;

    for( auto i: hist ){
        s.min = std::min( s.min, i );
        s.max = std::max( s.max, i );
        s.avg += i;
    }
    s.avg /= hist.size();

    s.std = 0;
    for( auto i: hist ){
        double scarto = i-s.avg;
        s.std += scarto*scarto;
    }
    s.std = std::sqrt(s.std/hist.size());

    return s;
}







#define NDELTA 50

// qui teniamo una lista delle durate
// verrà poi usata da k-means o un'altro algo di clustering
// per classificare punti, linee e spazi in base alla durata
std::vector<int> mark_delta;  // HEADER
std::vector<int> space_delta; // HEADER





double detect_eps = 0.81; // HEADER
double y_scale    = 1;   // HEADER



double sigmoid( double t ){
    double e = exp(t);
    return e/(1+e);
}




double integ_filt( double in ){
    static double y;
    static double dy;
    dy += (in-y)*0.01;
    dy *= 0.99;
    y += dy*0.01;
    return y;
}




bool on_mark_change( bool mark, int frame ){

    static bool prev_mark;
    static int  prev_frame;
    static int  idx;

    // edge sense
    if(!prev_mark && mark){
        // space → mark
        int delta = frame-prev_frame;
        fprintf(stderr,"space %d\n",delta);
        // salviamo
        if(0==space_delta.size())space_delta.resize(NDELTA);
        space_delta[idx%space_delta.size()]=delta;
        idx++;
    } else
    if(prev_mark && !mark){
        // mark → space
        int delta = frame-prev_frame;
        fprintf(stderr,"mark  %d\n",delta);
        // salviamo
        if(0==mark_delta.size())mark_delta.resize(NDELTA);
        mark_delta[idx%mark_delta.size()]=delta;
        idx++;
    }

    prev_mark  = mark;
    prev_frame = frame;
    return mark;
}





static double beep(){
    static double v = 0.5;
    return (v = -v);
}


static double shift( double buf[], int nbuf, double in ){
    const int end = nbuf-1;
    double last = buf[0];
    memmove( &buf[0], &buf[1], end*sizeof(buf[1]));
    buf[end] = in;
    return last;
}


static double integ( double buf[], int nbuf ){
    double sum = 0;
    for( int i=0; i<nbuf; i++ ){
        sum += buf[i];
    }
    return sum;
}











void on_fft( float* fft, int nfft ){ // HEADER

    static long frame;
    frame++;

    // dunque, proviamo un'approccio piu "ottico"
    // come sempre calcoliamo la flatness

    double geo = 1;
    double avg = 0;
    for( int i=0; i<nfft; i++ ){
        geo *= fft[i];
        avg += fft[i];
    }
    geo = std::pow( geo, 1/nfft );
    avg /= nfft;

    assert(std::isfinite(geo));
    assert(std::isfinite(avg));

//    double flatness = geo*30/(0.1+avg); // saregge geo/avg, ma lo 0.1 scongiura scomodi infiniti in casi speciali
//    fprintf(stderr,"flatness %f\n",flatness);
    double flatness = std::log(1+geo/(0.1+avg))*30;
//    double flatness = softknee(geo/avg*30);
    assert(std::isfinite(flatness));

    tracks.G.update( flatness*y_scale );


    // ok. ora popoliamo una fifo
    // qui verranno fatte le varie analisi per capire
    // quando parte e quando termina il mark
    // la risposta non è in tempo reale, non è un flusso:
    // si ha quando ogni "evento" accade come e quando dovrebbe
    // cmq in genere alla fine del mark
    static std::deque<double> fifo;
    fifo.push_front(flatness); // [0] = ora,  [1,2,3...] = passato

    // sicuramente evitiamo di andare in overflow
    // visto che misuriamo la lunghezza dei marks
    // ci serve una fifo che sia almeno lunga
    // quanto la feature più lunga che troveremo, o più
    // i mark son abbastanza brevi, quindi 1sec dovrebbe esser sufficente
    while(fifo.size()>FRAMES_FROM_MSEC(1000))fifo.pop_back();

    // ok. l'algoritmo lavora cosi:
    // partiamo da un punto arbitrario, diciamo 100ms nel passato
    // l'idea è di trovarsi in una "vasca", la zona di flatness bassa, quando c'è il tono
    // da li, facciamo una scansione verso il presente
    // per trovare quando la flatness sale sopra una soglia (fine del tono)
    // se la troviamo, ci segnamo il frame
    // stessa cosa andando nel passato
    // alla fine avremo due timestamps:
    // se la differenza è troppo piccola, lo consideriamo un fluke e non lo caghiamo
    // se invece inizio e fine hanno senso, si riporta il mark al decoder
    // e si svuota la fifo fino al frame finale del mark

    // la fifo evolve da destra a sinistra: 
    // passato a destra, presente a sinistra
    // fifo    [xxxxxxxxxxxxxxx]
    // indici   01234...      size-1
    // presente ↑             ↑ passato
    // quindi
    // push_front per inserire in indice 0, e
    // pop_back per scodare da size-1
    // procediamo:

    // inutile operare senza abbastanza storia
    const int search_start = FRAMES_FROM_MSEC(100);
    if(fifo.size()<search_start)return;

    // prima di tutto, il punto di partenza nel passato
    // deve stare dentro un range, il fondo della vasca
    if( fifo[search_start] > 0.7 )return;

    // timestamps
    const long NOT_FOUND = -1;
    long mark_end = NOT_FOUND;
    long mark_beg = NOT_FOUND;

    // ok. ora scorriamo dal passato al presente
    for( int i=search_start; i>=0; i-- ){
        if(fifo[i] > detect_eps ){
            mark_end = frame-i;
            break;
        }
    }


    if(mark_end==NOT_FOUND)return;






    // ora scorriamo dal passato al passato remoto
    for( int i=search_start; i<fifo.size(); i++ ){
        if(fifo[i] > detect_eps ){
            mark_beg = frame-i;
            break;
        }
    }

    if(mark_beg==NOT_FOUND)return;

    // abbiamo un potenziale mark
    // però deve avere una lunghezza minima (quella del punto o giu di li)
    // potremmo discriminare ancora piu aggressivamente
    // accettando solo certe lunghezze
    int mark_len = mark_end - mark_beg;
    assert(mark_len>=0);
    if(mark_len<FRAMES_FROM_MSEC( 30 ))return; 

    // ok abbiamo un mark valido
    // bruciamo la storia fino al frame che termina il mark
    while(fifo.size()>(frame-mark_end))fifo.pop_back();

    fprintf(stderr,"mark @ %ld len %d\n"
        ,MSEC_FROM_FRAMES(mark_beg)
        ,MSEC_FROM_FRAMES(mark_len)
    );

    // queste info vengono passate al discriminatore punti-linee-spazio
    // che poi tradurrà le sequenze in testo tramite il dizionario
}







