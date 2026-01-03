#ifndef STRUCT_H
#define STRUCT_H
#include "struct.h"

Case **creerplateau();

// Helper to initialize Partie with default values
static inline void init_partie(Partie *p) {
    p->dernier_coup.iFrom = -1;
    p->dernier_coup.iTo = -1;
    p->dernier_coup.jFrom = -1;
    p->dernier_coup.jTo = -1;
    // Initialize castling/movement flags
    for (int c = 0; c < 2; c++) {
        p->roi_moved[c] = false;
        for (int r = 0; r < 2; r++) p->rook_moved[c][r] = false;
    }
}

#endif // STRUCT_H
