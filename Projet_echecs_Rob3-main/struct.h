#ifndef struct_h
#define struct_h

#include <stdbool.h>

typedef enum { tour, cavalier, fou, reine, roi, pion, vide } Piece;
typedef enum { blanc, noir, couleurvide } Couleur;

typedef struct {
    Piece p;
    Couleur c;
} Case;

typedef struct {
    Piece p;
    char pos[8];
} Tab;

typedef struct {
    int iFrom;
    int jFrom;
    int iTo;
    int jTo;
} Coup;

typedef struct {
    Case** plateau;
    Couleur joueur_actif;
    int nb_coups;
    int temps_blanc;
    int temps_noir;
    Coup dernier_coup;  // Track last move for en passant detection
    // Castling/movement flags: index 0 = blanc, 1 = noir
    bool roi_moved[2];
    // rook_moved[color][0]=queen-side (col 0), [1]=king-side (col 7)
    bool rook_moved[2][2];
} Partie;
#endif
