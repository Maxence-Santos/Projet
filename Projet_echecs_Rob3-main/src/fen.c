#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fen.h"

// Map enum Piece to char
static char piece_char(Piece p) {
    switch (p) {
        case tour: return 'r';
        case cavalier: return 'n';
        case fou: return 'b';
        case reine: return 'q';
        case roi: return 'k';
        case pion: return 'p';
        default: return '?';
    }
}

char* generate_fen(const Partie* partie) {
    if (!partie || !partie->plateau) return NULL;

    // Buffer raisonnable pour FEN basique
    size_t cap = 256;
    char* out = (char*)malloc(cap);
    if (!out) return NULL;
    out[0] = '\0';

    for (int i = 0; i < 8; ++i) {
        int empty = 0;
        for (int j = 0; j < 8; ++j) {
            Case c = partie->plateau[i][j];
            if (c.p == vide) {
                ++empty;
            } else {
                if (empty > 0) {
                    char tmp[4];
                    snprintf(tmp, sizeof(tmp), "%d", empty);
                    if (strlen(out) + strlen(tmp) + 1 >= cap) {
                        cap *= 2; out = (char*)realloc(out, cap);
                    }
                    strcat(out, tmp);
                    empty = 0;
                }
                char pc = piece_char(c.p);
                if (c.c == blanc) pc = toupper((unsigned char)pc);
                size_t len = strlen(out);
                if (len + 2 >= cap) { cap *= 2; out = (char*)realloc(out, cap); }
                out[len] = pc;
                out[len+1] = '\0';
            }
        }
        if (empty > 0) {
            char tmp[4];
            snprintf(tmp, sizeof(tmp), "%d", empty);
            if (strlen(out) + strlen(tmp) + 1 >= cap) { cap *= 2; out = (char*)realloc(out, cap); }
            strcat(out, tmp);
        }
        if (i != 7) {
            if (strlen(out) + 2 >= cap) { cap *= 2; out = (char*)realloc(out, cap); }
            strcat(out, "/");
        }
    }

    // Côté à jouer : joueur_actif est le joueur dont c'est le tour de jouer
    char side = (partie->joueur_actif == blanc) ? 'w' : 'b';

    // En passant : vérifier si le dernier coup était une avancée de pion de deux cases
    char ep[4] = "-";
    if (partie->dernier_coup.iFrom >= 0 && partie->dernier_coup.iTo >= 0) {
        int di = partie->dernier_coup.iTo - partie->dernier_coup.iFrom;
        // Vérifier si c'était un pion avançant de 2 cases
        if ((partie->joueur_actif == blanc && di == 2) || (partie->joueur_actif == noir && di == -2)) {
            // La case en passant est la case entre from et to
            int ep_row = (partie->dernier_coup.iFrom + partie->dernier_coup.iTo) / 2;
            int ep_col = partie->dernier_coup.jTo;
            snprintf(ep, sizeof(ep), "%c%d", 'a' + ep_col, 8 - ep_row);
        }
    }

    // Ajouter le côté à jouer, le roque (toujours "-" pour l'instant), l'en passant, et les compteurs de coups
    if (strlen(out) + 32 >= cap) { cap *= 2; out = (char*)realloc(out, cap); }
    char tail[64];
    snprintf(tail, sizeof(tail), " %c - %s 0 1", side, ep);
    strcat(out, tail);

    return out;
}
