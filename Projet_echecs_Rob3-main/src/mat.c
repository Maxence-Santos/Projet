#include "../mat.h"

bool est_en_mat(Partie partie, Couleur joueur_verif) {
    int iroi, jroi;
    
    // Balayage du plateau pour trouver le roi du joueur actif

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (partie.plateau[i][j].p == roi && partie.plateau[i][j].c == joueur_verif) {
                iroi = i;
                jroi = j;
            }
        }
    }
    Coup coup;
    Partie partie2;
    Tab tableau;
    partie2.joueur_actif = joueur_verif;
    int coup_possible[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int i = 0; i < 8; i++) {
        coup.iFrom = iroi;
        coup.jFrom = jroi;
        coup.iTo = iroi + coup_possible[i][0];
        coup.jTo = jroi + coup_possible[i][1];

        if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8) {
            if (est_mouvement_valide(coup, roi, partie, joueur_verif)) {
                partie2 = copie_tableau(partie);
                appliquer_coup(&partie2, coup, &tableau, 1);
                if (!est_en_echec(partie2, joueur_verif)) {
                    return false;
                }
                libererTableau(partie2);
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((partie.plateau[i][j].p == pion || partie.plateau[i][j].p == reine ||partie.plateau[i][j].p == tour || partie.plateau[i][j].p == fou || partie.plateau[i][j].p == cavalier) && partie.plateau[i][j].c == joueur_verif) {
                coup.iFrom = i;
                coup.jFrom = j;
                for (int k = 0; k < 8; k++) {
                    for (int l = 0; l < 8; l++) {
                        coup.iTo = k;
                        coup.jTo = l;
                        if (est_mouvement_valide(coup, partie.plateau[i][j].p, partie, joueur_verif)) {
                            partie2 = copie_tableau(partie);
                            appliquer_coup(&partie2, coup, &tableau, 1);
                            if (!est_en_echec(partie2, joueur_verif)) {
                                return false;
                            }
                            libererTableau(partie2);
                        }
                    }
                }
            }
        }
    }
    return true;
}