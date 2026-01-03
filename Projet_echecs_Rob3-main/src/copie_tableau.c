#include <stdbool.h>
#include <math.h>
#include "../copie_tableau.h"

Partie copie_tableau(Partie partie){
    Partie copie;
    copie.plateau = malloc(8 * sizeof(Case*));
    for (int i = 0; i < 8; i++) {
        copie.plateau[i] = malloc(8 * sizeof(Case));
        for (int j = 0; j < 8; j++) {
            copie.plateau[i][j] = partie.plateau[i][j];
        }
    }
    copie.joueur_actif = partie.joueur_actif;
    copie.nb_coups = partie.nb_coups;
    copie.temps_blanc = partie.temps_blanc;
    copie.temps_noir = partie.temps_noir;
    copie.dernier_coup = partie.dernier_coup;
    // Copier les flags de roque et de mouvement
    for (int c = 0; c < 2; c++) {
        copie.roi_moved[c] = partie.roi_moved[c];
        for (int r = 0; r < 2; r++) copie.rook_moved[c][r] = partie.rook_moved[c][r];
    }
    return copie;
}

void libererTableau(Partie tab) {
    for (int i = 0; i < 8; i++) {
        free(tab.plateau[i]);
    }
    free(tab.plateau);
}