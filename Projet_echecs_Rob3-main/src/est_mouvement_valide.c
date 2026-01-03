#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "../est_mouvement_valide.h"
#include "../copie_tableau.h"

bool est_mouvement_valide(Coup coup, Piece type_piece, Partie partie, Couleur joueur_actif) {
    int di = abs(coup.iTo - coup.iFrom);
    int dj = abs(coup.jTo - coup.jFrom);
    if (di == 0 && dj == 0) {
        return false;
    }
    if (coup.iFrom < 0 || coup.iFrom > 7 || coup.jFrom < 0 || coup.jFrom > 7 || coup.iTo < 0 || coup.iTo > 7 || coup.jTo < 0 || coup.jTo > 7) {
        return false;
    }

    switch (type_piece) {
        case roi:
            // Vérifier la case d'arrivée
            if (partie.plateau[coup.iTo][coup.jTo].p != vide && partie.plateau[coup.iTo][coup.jTo].c == joueur_actif) {
                return false;
            }
            // Mouvement standard d'une case
            if (di <= 1 && dj <= 1) {
                return true;
            }

            // Roque: le roi se déplace de deux cases horizontalement
            if (di == 0 && dj == 2) {
                int row = coup.iFrom;
                // Le roi ne doit pas être en échec actuellement
                if (est_en_echec(partie, joueur_actif)) return false;

                // Vérifier que ni le roi ni la tour n'ont déjà bougé
                int color_idx = (joueur_actif == blanc) ? 0 : 1;
                if (partie.roi_moved[color_idx]) return false;

                // Roque côté roi (petit roque)
                if (coup.jTo > coup.jFrom) {
                    if (partie.rook_moved[color_idx][1]) return false;
                    // Vérifier la présence d'une tour à l'extrémité
                    if (partie.plateau[row][7].p != tour || partie.plateau[row][7].c != joueur_actif) return false;
                    // Les cases entre le roi et la tour doivent être libres
                    for (int j = coup.jFrom + 1; j <= 6; j++) {
                        if (partie.plateau[row][j].p != vide) return false;
                    }
                    // Le roi ne doit pas traverser ni finir sur une case contrôlée
                    for (int j = coup.jFrom + 1; j <= coup.jTo; j++) {
                        Partie tmp = copie_tableau(partie);
                        tmp.plateau[row][j] = tmp.plateau[row][coup.jFrom];
                        tmp.plateau[row][coup.jFrom].p = vide;
                        tmp.plateau[row][coup.jFrom].c = couleurvide;
                        if (est_en_echec(tmp, joueur_actif)) {
                            libererTableau(tmp);
                            return false;
                        }
                        libererTableau(tmp);
                    }
                    return true;
                }

                // Roque côté dame (grand roque)
                else {
                    if (partie.rook_moved[color_idx][0]) return false;
                    if (partie.plateau[row][0].p != tour || partie.plateau[row][0].c != joueur_actif) return false;
                    // Les cases entre le roi et la tour doivent être libres
                    for (int j = 1; j < coup.jFrom; j++) {
                        if (partie.plateau[row][j].p != vide) return false;
                    }
                    // Le roi ne doit pas traverser ni finir sur une case contrôlée
                    for (int j = coup.jFrom - 1; j >= coup.jTo; j--) {
                        Partie tmp = copie_tableau(partie);
                        tmp.plateau[row][j] = tmp.plateau[row][coup.jFrom];
                        tmp.plateau[row][coup.jFrom].p = vide;
                        tmp.plateau[row][coup.jFrom].c = couleurvide;
                        if (est_en_echec(tmp, joueur_actif)) {
                            libererTableau(tmp);
                            return false;
                        }
                        libererTableau(tmp);
                    }
                    return true;
                }
            }

            return false;

        case reine:
            if (di == 0 || dj == 0) { // Si la reine se déplace en ligne droite
                int step_i = (di == 0) ? 0 : (coup.iTo - coup.iFrom) / di;
                int step_j = (dj == 0) ? 0 : (coup.jTo - coup.jFrom) / dj;
                for (int i = coup.iFrom + step_i, j = coup.jFrom + step_j; i != coup.iTo || j != coup.jTo; i += step_i, j += step_j) {
                    if (partie.plateau[i][j].p != vide) {
                        return false;
                    }
                }
            } 
            else if (di == dj) { // Si la reine se déplace en diagonale
                int step_i = (coup.iTo - coup.iFrom) / di;
                int step_j = (coup.jTo - coup.jFrom) / dj;
                for (int i = coup.iFrom + step_i, j = coup.jFrom + step_j; i != coup.iTo && j != coup.jTo; i += step_i, j += step_j) {
                    if (partie.plateau[i][j].p != vide) {
                        return false;
                    }
                }
            }
            // Vérifier la case d'arrivée
            if (partie.plateau[coup.iTo][coup.jTo].p != vide && partie.plateau[coup.iTo][coup.jTo].c == joueur_actif) {
                return false;
            }
            return (di == dj || di == 0 || dj == 0);

        case tour:
            if (di == 0 || dj == 0) { // Si la tour se déplace en ligne droite
                int step_i = (di == 0) ? 0 : (coup.iTo - coup.iFrom) / di;
                int step_j = (dj == 0) ? 0 : (coup.jTo - coup.jFrom) / dj;
                for (int i = coup.iFrom + step_i, j = coup.jFrom + step_j; i != coup.iTo || j != coup.jTo; i += step_i, j += step_j) {
                    if (partie.plateau[i][j].p != vide) {
                        return false;
                    }
                }
            }
            // Vérifier la case d'arrivée
            if (partie.plateau[coup.iTo][coup.jTo].p != vide && partie.plateau[coup.iTo][coup.jTo].c == joueur_actif) {
                return false;
            }
            return (di == 0 || dj == 0);

        case fou:
            if (di == dj) { // Si le fou se déplace en diagonale
                int step_i = (coup.iTo - coup.iFrom) / di;
                int step_j = (coup.jTo - coup.jFrom) / dj;
                for (int i = coup.iFrom + step_i, j = coup.jFrom + step_j; i != coup.iTo && j != coup.jTo; i += step_i, j += step_j) {
                    if (partie.plateau[i][j].p != vide) {
                        return false;
                    }
                }
            }
            // Vérifier la case d'arrivée
            if (partie.plateau[coup.iTo][coup.jTo].p != vide && partie.plateau[coup.iTo][coup.jTo].c == joueur_actif) {
                return false;
            }
            return (di == dj);

        case cavalier:
            // Vérifier la case d'arrivée
            if (partie.plateau[coup.iTo][coup.jTo].p != vide && partie.plateau[coup.iTo][coup.jTo].c == joueur_actif) {
                return false;
            }
            return (di == 2 && dj == 1) || (di == 1 && dj == 2);

        case pion:
            // Le pion se déplace d'une case en avant ou capture en diagonale (vers l'avant uniquement)
            if (dj == 0 && partie.plateau[coup.iTo][coup.jTo].p == vide) {
                // Mouvement avant d'une case
                if (blanc == joueur_actif && coup.iTo == coup.iFrom - 1 && di == 1) {
                    return true;
                }
                else if (noir == joueur_actif && coup.iTo == coup.iFrom + 1 && di == 1) {
                    return true;
                }
            } else if (di == 1 && dj == 1) {
                // Vérifier que la capture se fait vers l'avant selon la couleur
                if (blanc == joueur_actif && coup.iTo != coup.iFrom - 1) {
                    return false;
                }
                if (noir == joueur_actif && coup.iTo != coup.iFrom + 1) {
                    return false;
                }
                // Capture diagonale normale
                if (partie.plateau[coup.iTo][coup.jTo].p != vide && partie.plateau[coup.iTo][coup.jTo].c != joueur_actif) {
                    return true;
                }
                // Capture en passant: destination vide, mais ennemi sur la même ligne que l'attaquant et colonne cible
                if (partie.plateau[coup.iTo][coup.jTo].p == vide) {
                    int enemy_row = coup.iFrom;     // ligne de l'ennemi (même ligne que l'attaquant)
                    int enemy_col = coup.jTo;       // colonne de l'ennemi (colonne cible)
                    if (partie.plateau[enemy_row][enemy_col].p == pion && partie.plateau[enemy_row][enemy_col].c != joueur_actif) {
                        // Vérifier que ce pion ennemi vient de faire un double pas
                        if (partie.dernier_coup.iTo == enemy_row && partie.dernier_coup.jTo == enemy_col) {
                            int edi = partie.dernier_coup.iFrom - partie.dernier_coup.iTo;
                            if (edi == 2 || edi == -2) {  // Pion enemi bougé de 2 cases
                                return true;  // En passant est valide
                            }
                        }
                    }
                }
            }
            // Le pion peut se déplacer de deux cases en avant s'il n'a pas encore bougé
            if (di == 2 && dj == 0 && noir == joueur_actif && coup.iFrom == 1 && coup.iTo == 3 && partie.plateau[2][coup.jTo].p == vide && partie.plateau[coup.iTo][coup.jTo].p == vide) {
                return true;
            }
            if (di == 2 && dj == 0 && blanc == joueur_actif && coup.iFrom == 6 && coup.iTo == 4 && partie.plateau[5][coup.jTo].p == vide && partie.plateau[coup.iTo][coup.jTo].p == vide) {
                return true;
            }

            return false;

        default:
            return false;
    }
}

bool mouvement_echecia(Coup coup, Partie partie, Tab* tableau) {
    Partie partie2;
    partie2 = copie_tableau(partie);
    appliquer_coup(&partie2, coup, NULL, 1);

        // Vérifier si le roi est en échec après le mouvement
        if (est_en_echec(partie2, partie.joueur_actif)) {
            libererTableau(partie2);
            return false;
        }
        libererTableau(partie2);
        return true;
}