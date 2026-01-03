#include "../appliquercoup.h"
#include "../struct.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void appliquer_coup(Partie* partie, Coup coup, Tab* tab, int ia) {
    // Save moving piece info for later (before modifying the board)
    Piece moving_piece = partie->plateau[coup.iFrom][coup.jFrom].p;
    Couleur moving_color = partie->plateau[coup.iFrom][coup.jFrom].c;
    
    // Détection en passant: mouvement diagonal d'un pion vers une case vide
    int est_en_passant = 0;
    if (partie->plateau[coup.iFrom][coup.jFrom].p == pion &&
        coup.iFrom != coup.iTo && coup.jFrom != coup.jTo &&  // diagonal
        partie->plateau[coup.iTo][coup.jTo].p == vide) {  // destination empty
        est_en_passant = 1;
    }
    
    // Concaténation des positions pour mise à jour dans `tab`
    char piece_dep[8] = {coup.jFrom + 'A', coup.iFrom + '1', '\0'};
    char piece_arr[8] = {coup.jTo + 'A', coup.iTo + '1', '\0'};
    snprintf(piece_dep, 4, "%c%c", coup.jFrom + 'A', coup.iFrom + '1');
    snprintf(piece_arr, 4, "%c%c", coup.jTo + 'A', coup.iTo + '1');

    // Détection d'une capture (normal ou en passant)
    int est_capture = (partie->plateau[coup.iTo][coup.jTo].p != vide) || est_en_passant;
    if (est_capture && tab != NULL) {
        // Détermine la couleur de la pièce capturée (avant le déplacement)
        Couleur capture_couleur = couleurvide;
        if (est_en_passant) {
            capture_couleur = partie->plateau[coup.iFrom][coup.jTo].c;
        }
        else {
            capture_couleur = partie->plateau[coup.iTo][coup.jTo].c;
        }

        // Supprime du tableau `tab` si la pièce capturée est noire
        if (capture_couleur == noir) {
            char captured_pos[8];
            if (est_en_passant) {
                snprintf(captured_pos, 4, "%c%c", coup.jTo + 'A', coup.iFrom + '1');
            }
            else {
                snprintf(captured_pos, 4, "%c%c", coup.jTo + 'A', coup.iTo + '1');
            }
            for (int i = 0; i < 16; i++) {
                if (strcmp(tab[i].pos, captured_pos) == 0) {
                    for (int k = i; k < 15; k++) {
                        tab[k] = tab[k+1];
                    }
                    tab[15].p = vide;
                    tab[15].pos[0] = '\0';
                    break;
                }
            }
        }
    }

    // Bouger la pièce sur le plateau
    partie->plateau[coup.iTo][coup.jTo] = partie->plateau[coup.iFrom][coup.jFrom];
    partie->plateau[coup.iFrom][coup.jFrom].p = vide;
    partie->plateau[coup.iFrom][coup.jFrom].c = couleurvide;

    if (moving_color == noir && tab != NULL) {
        // mise à jour de tab pour la pièce déplacée
        for (int i = 0; i < 16; i++) {
            if (strcmp(tab[i].pos, piece_dep) == 0) {
                strcpy(tab[i].pos, piece_arr);
                break;
            }
        }
    }

    // Pour le en passant, supprimer le pion ennemi à (iFrom, jTo) sur le plateau
    if (est_en_passant) {
        partie->plateau[coup.iFrom][coup.jTo].p = vide;
        partie->plateau[coup.iFrom][coup.jTo].c = couleurvide;
    }
    
    // Promotion des pions
    if (ia != 0) {
        if ((coup.iTo == 0 || coup.iTo == 7) && partie->plateau[coup.iTo][coup.jTo].p == pion) {
            partie->plateau[coup.iTo][coup.jTo].p = reine;
        }
    }

    // Gestion du roque: si le roi a effectué un déplacement de deux colonnes, déplacer la tour correspondante
    if (moving_piece == roi && abs(coup.jTo - coup.jFrom) == 2) {
        int col = coup.iTo;
        int roque_dep_col, roque_vers_col;
        if (coup.jTo > coup.jFrom) {
            // petit roque: tour de la colonne 7 vers jTo-1
            roque_dep_col = 7;
            roque_vers_col = coup.jTo - 1;
        } else {
            // grand roque: tour de la colonne 0 vers jTo+1
            roque_dep_col = 0;
            roque_vers_col = coup.jTo + 1;
        }

        // Déplacer la tour sur le plateau
        Case rook_case = partie->plateau[col][roque_dep_col];
        partie->plateau[col][roque_vers_col] = rook_case;
        partie->plateau[col][roque_dep_col].p = vide;
        partie->plateau[col][roque_dep_col].c = couleurvide;

        if (moving_color == noir && tab != NULL) {
            // Mettre à jour `tab` pour la tour
            char roque_dep[8];
            char roque_vers[8];
            snprintf(roque_dep, 4, "%c%c", roque_dep_col + 'A', col + '1');
            snprintf(roque_vers, 4, "%c%c", roque_vers_col + 'A', col + '1');
            for (int i = 0; i < 16; i++) {
                if (strcmp(tab[i].pos, roque_dep) == 0) {
                    strcpy(tab[i].pos, roque_vers);
                    break;
                }
            }
        }

        // Marquer la tour comme déplacée pour empêcher un roque ultérieur
        int color_index = (moving_color == blanc) ? 0 : 1;
        if (roque_dep_col == 0) partie->rook_moved[color_index][0] = true; else partie->rook_moved[color_index][1] = true;
    }

    // Marquer le roi/tour comme déplacé si nécessaire
    if (moving_piece == roi) {
        int color_index = (moving_color == blanc) ? 0 : 1;
        partie->roi_moved[color_index] = true;
    }
    if (moving_piece == tour) {
        int color_index = (moving_color == blanc) ? 0 : 1;
        if (coup.jFrom == 0) partie->rook_moved[color_index][0] = true;
        if (coup.jFrom == 7) partie->rook_moved[color_index][1] = true;
    }
    // Enregistrer ce coup comme dernier coup (utile pour la détection d'en-passant du prochain joueur)
    partie->dernier_coup = coup;
}
