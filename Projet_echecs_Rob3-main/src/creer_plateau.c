#include <stdlib.h>
#include "../struct.h"

// Implémentation de la fonction pour créer le plateau
Case **creerplateau() {
    // Allocation de mémoire pour le plateau 8x8
    Case **plateau = (Case **)malloc(8 * sizeof(Case *));
    for (int i = 0; i < 8; i++) {
        plateau[i] = (Case *)malloc(8 * sizeof(Case));
    }

    // Initialisation des pièces noires
    plateau[0][0].p = tour; plateau[0][1].p = cavalier; plateau[0][2].p = fou;
    plateau[0][3].p = reine; plateau[0][4].p = roi; plateau[0][5].p = fou;
    plateau[0][6].p = cavalier; plateau[0][7].p = tour;

    for (int j = 0; j < 8; j++) {
        plateau[0][j].c = noir;
        plateau[1][j].p = pion;
        plateau[1][j].c = noir;
    }

    // Initialisation des cases vides
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            plateau[i][j].p = vide;
            plateau[i][j].c = couleurvide;
        }
    }

    // Initialisation des pièces blanches
    for (int j = 0; j < 8; j++) {
        plateau[6][j].p = pion;
        plateau[6][j].c = blanc;
    }

    plateau[7][0].p = tour; plateau[7][1].p = cavalier; plateau[7][2].p = fou;
    plateau[7][3].p = reine; plateau[7][4].p = roi; plateau[7][5].p = fou;
    plateau[7][6].p = cavalier; plateau[7][7].p = tour;

    for (int j = 0; j < 8; j++) {
        plateau[7][j].c = blanc;
    }

    return plateau;
}