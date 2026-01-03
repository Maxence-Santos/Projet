#include "../est_en_echec.h"
#include "../est_mouvement_valide.h"
#include <stdio.h>

bool est_en_echec(Partie partie, Couleur joueur_actif) {
    int iroi, jroi, autrejoueur;
    if (joueur_actif == blanc) {
        autrejoueur = noir;
    }
    else {
        autrejoueur = blanc;
    }
    // Balayage du plateau pour trouver le roi du joueur actif
    int t = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (partie.plateau[i][j].p == roi && partie.plateau[i][j].c == joueur_actif) {
                iroi = i;
                jroi = j;
                t = 1;
            }
        }
    }
    if (t == 0) {
        return true;
    }
    // Balayage du plateau pour trouver les pièces adverses

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (t == 1 && partie.plateau[i][j].p != vide && partie.plateau[i][j].c != joueur_actif) {
                Coup coup;
                coup.iFrom = i;
                coup.jFrom = j;
                coup.iTo = iroi;
                coup.jTo = jroi;
                // Passe la vérification du roque pour éviter une récursion infinie
                // (un roi ne peut jamais être en échec à cause d'un mouvement de roque de toute façon)
                if (partie.plateau[i][j].p == roi && abs(coup.jTo - coup.jFrom) == 2) {
                    continue;
                }
                if (est_mouvement_valide(coup, partie.plateau[i][j].p, partie, autrejoueur)) {
                    return true;
                }
            }
        }
    }
    return false;
}