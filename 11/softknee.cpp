
#undef NDEBUG
#include <assert.h>




float softknee( float in ){   // HEADER
    // un semplice limiter:
    // 0  → 0
    // ±1 → ±0.5
    // ±∞ → ±1
    // OKKIO: va proprio usato in>=0
    // con in>0 va in segfault, credo parta una ricorsione infinita
    // per in=-0, che è sia negativo che =0... magie dello IEEE
    // invece con in>=0 viene agganciato il -0 come fosse =0 e va tutto bene
    if(in>=0) return in/(1+in);
    return -softknee(-in);
}



