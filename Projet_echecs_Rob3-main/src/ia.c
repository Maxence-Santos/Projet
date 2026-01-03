#include "../ia.h"

// Variable globale pour suivre le nombre de coups légaux depuis le dernier appel à proposition_ia
int g_last_compt = 0;

Coup proposition_ia(Partie partie, Tab *tableau) {
    Coup coup;
    Coup coup_courant;
    srand( time( NULL ) );
    Partie partie_copie;
    int joueuria = partie.joueur_actif;
    int meilleursscore = -1000;
    int meilleursscore2 = -1000;
    int compt = 0;
    coup_courant.iFrom = 0;
    coup_courant.jFrom = 0;
    coup_courant.iTo = 0;
    coup_courant.jTo = 0;
    
    // BRUTE-FORCE ENUMERATION: Test de tous les mouvements possibles (ORIGINAL - WORKING VERSION)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (partie.plateau == NULL) {
                continue;
            }
            if (partie.plateau[i] == NULL) {
                continue;
            }
            if (partie.plateau[i][j].c == partie.joueur_actif) {
                Piece piece_type = partie.plateau[i][j].p;
                if (piece_type == pion) {
                    // Mouvement des pions : avant, avant, avant-droite et double pas en avant
                    int dirs[3] = {-1, 0, 1};
                    for (int d = 0; d < 3; d++) {
                        int tj = j + dirs[d];
                        int ti = (partie.joueur_actif == noir) ? i + 1 : i - 1;
                        if (ti >= 0 && ti < 8 && tj >= 0 && tj < 8) {
                            coup.iFrom = i; coup.jFrom = j; coup.iTo = ti; coup.jTo = tj;
                            if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, piece_type, partie, partie.joueur_actif)) {
                                compt++;
                                partie_copie = copie_tableau(partie);
                                appliquer_coup(&partie_copie, coup, NULL, 1);
                                partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                                meilleursscore2 = meilleurscoup1(partie_copie, joueuria, tableau);
                                if (meilleursscore2 > meilleursscore || (meilleursscore2 == meilleursscore && rand() % 5 == 0)) {
                                    meilleursscore = meilleursscore2;
                                    coup_courant = coup;
                                }
                                libererTableau(partie_copie);
                            }
                        }
                    }
                    // Double-pas
                    int ti2 = (partie.joueur_actif == noir) ? i + 2 : i - 2;
                    if (ti2 >= 0 && ti2 < 8) {
                        coup.iFrom = i; coup.jFrom = j; coup.iTo = ti2; coup.jTo = j;
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, piece_type, partie, partie.joueur_actif)) {
                            compt++;
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, NULL, 1);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            meilleursscore2 = meilleurscoup1(partie_copie, joueuria, tableau);
                            if (meilleursscore2 > meilleursscore || (meilleursscore2 == meilleursscore && rand() % 5 == 0)) {
                                meilleursscore = meilleursscore2;
                                coup_courant = coup;
                            }
                            libererTableau(partie_copie);
                        }
                    }
                } else {
                    for (int ti = 0; ti < 8; ti++) {
                        for (int tj = 0; tj < 8; tj++) {
                            if (i == ti && j == tj) continue;  // Saute la même case

                            coup.iFrom = i;
                            coup.jFrom = j;
                            coup.iTo = ti;
                            coup.jTo = tj;

                            // Vérifie si le mouvement est légal selon les règles de déplacement des pièces et la sécurité du roi
                            int ok_mvt = mouvement_echecia(coup, partie, NULL);
                            if (ok_mvt && 
                                est_mouvement_valide(coup, piece_type, partie, partie.joueur_actif)) {
                                compt++;
                                
                                // Evalue ce coup
                                partie_copie = copie_tableau(partie);
                                appliquer_coup(&partie_copie, coup, NULL, 1);
                                partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                                meilleursscore2 = meilleurscoup1(partie_copie, joueuria, tableau);
                                if (meilleursscore2 > meilleursscore) {
                                    meilleursscore = meilleursscore2;
                                    coup_courant = coup;
                                }
                                if (meilleursscore2 == meilleursscore) {
                                    if (rand() % 5 == 0) {
                                        meilleursscore = meilleursscore2;
                                        coup_courant = coup;
                                    }
                                }
                                libererTableau(partie_copie);
                            }
                        }
                    }
                }
            }
        }
    }

    g_last_compt = compt;  // Stocke compt pour un accès externe
    return coup_courant;
}

int meilleurscoup1(Partie partie, int joueuria, Tab *tableau){
    Coup coup;
    Partie partie_copie;
    int score = 0;
    int moyenne = 0;
    int m = 0;
    for (int i=0; i < 16; i++){
        if (partie.joueur_actif == noir) {
            if (tableau[i].p == pion){
                // Test mouvements d'un pas des pions
                for (int k = 0; k<3;k++){
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.jTo = tableau[i].pos[1]+-1+k;
                    coup.iTo = (partie.joueur_actif == noir) ? tableau[i].pos[0]+1 : tableau[i].pos[0]-1;
                    if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)){
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup2(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                    }
                }
                // Test mouvements de deux pas des pions
                coup.iFrom = tableau[i].pos[0];
                coup.jFrom = tableau[i].pos[1];
                coup.jTo = tableau[i].pos[1];
                coup.iTo = (partie.joueur_actif == noir) ? tableau[i].pos[0]+2 : tableau[i].pos[0]-2;
                if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)){
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup2(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (tableau[i].p == reine) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = k;
                    coup.jTo = tableau[i].pos[1];
                    if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup2(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0];
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup2(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            
            }
            else if (tableau[i].p == tour) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = k;
                    coup.jTo = tableau[i].pos[1];
                    if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup2(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0];
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup2(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (tableau[i].p == fou) {
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
            else if (tableau[i].p == cavalier) {
                int deplacements[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0] + deplacements[k][0];
                    coup.jTo = tableau[i].pos[1] + deplacements[k][1];
                        if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
            else if (tableau[i].p == roi) {
                for (int k = 0; k < 3; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0] + k - 1;
                    coup.jTo = tableau[i].pos[1] + k - 1;
                        if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
                // Ajouter les possibilités de roque (déplacement du roi de 2 colonnes)
                coup.iFrom = tableau[i].pos[0];
                coup.jFrom = tableau[i].pos[1];
                for (int dj = -2; dj <= 2; dj += 4) {
                    coup.iTo = coup.iFrom;
                    coup.jTo = coup.jFrom + dj;
                    if (0 <= coup.jTo && coup.jTo <= 7) {
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup2(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
        }
    }
    if (m == 0){
        return -100;
    }
    return moyenne/m;
}

int meilleurscoup2(Partie partie, int joueuria, Tab* tableau){
    Coup coup;
    Partie partie_copie;
    int moyenne = 0;
    int score = 0;
    int m = 0;
    for (int i=0; i < 16; i++){
        if (partie.joueur_actif == noir) {
            if (tableau[i].p == pion){
                // Teste mouvements d'un pas des pions
                for (int k = 0; k<3;k++){
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.jTo = tableau[i].pos[1]+-1+k;
                    coup.iTo = (partie.joueur_actif == noir) ? tableau[i].pos[0]+1 : tableau[i].pos[0]-1;
                    if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)){
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup4(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                    }
                }
                // Teste mouvements de deux pas des pions
                coup.iFrom = tableau[i].pos[0];
                coup.jFrom = tableau[i].pos[1];
                coup.jTo = tableau[i].pos[1];
                coup.iTo = (partie.joueur_actif == noir) ? tableau[i].pos[0]+2 : tableau[i].pos[0]-2;
                if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)){
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup4(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (tableau[i].p == reine) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = k;
                    coup.jTo = tableau[i].pos[1];
                    if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup4(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0];
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup4(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup4(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup4(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            
            }
            else if (tableau[i].p == tour) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = k;
                    coup.jTo = tableau[i].pos[1];
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup4(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0];
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        score = meilleurscoup4(partie_copie,joueuria,tableau);
                        moyenne += score;
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (tableau[i].p == fou) {
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup4(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup4(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
            else if (tableau[i].p == cavalier) {
                int deplacements[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0] + deplacements[k][0];
                    coup.jTo = tableau[i].pos[1] + deplacements[k][1];
                        if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup4(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
            else if (tableau[i].p == roi) {
                for (int k = 0; k < 3; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0] + k - 1;
                    coup.jTo = tableau[i].pos[1] + k - 1;
                        if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, NULL) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            score = meilleurscoup4(partie_copie,joueuria,tableau);
                            moyenne += score;
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            } 
        }
        
    }
    if (m == 0){
        return -100;
    }
    return moyenne/m;

}

/*int meilleurscoup3(Partie partie, int joueuria){ TROP DE CALCULS
    Coup coup;
    Partie partie_copie;
    int moyenne = 0;
    int m = 0;
    for (int i=0; i < 8; i++){
        for (int j=0; j < 8; j++){
            if (partie.plateau[i][j].c == partie.joueur_actif){
                if (partie.plateau[i][j].p == pion){
                    for (int k = 0; k<3;k++){
                        coup.iFrom = i;
                        coup.jFrom = j;
                        coup.jTo = j+-1+k;
                        coup.iTo = (partie.joueur_actif == noir) ? i+1 : i-1;
                        if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                        if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)){
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup,1);
                            moyenne += meilleurscoup4(partie_copie,joueuria);
                            m++;
                            libererTableau(partie_copie);
                        }
                        }
                    }
                }
            else if (partie.plateau[i][j].p == reine) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = i;
                    coup.jFrom = j;
                    coup.iTo = k;
                    coup.jTo = j;
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = i;
                    coup.jFrom = j;
                    coup.iTo = i;
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = i;
                    coup.jFrom = j;
                    coup.iTo = i+k;
                    coup.jTo = j+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup,1);
                            moyenne += meilleurscoup4(partie_copie,joueuria);
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = i+k;
                    coup.jTo = j-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup,1);
                            moyenne += meilleurscoup4(partie_copie,joueuria);
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            
            }
            else if (partie.plateau[i][j].p == tour) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = i;
                    coup.jFrom = j;
                    coup.iTo = k;
                    coup.jTo = j;
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = i;
                    coup.jFrom = j;
                    coup.iTo = i;
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (partie.plateau[i][j].p == fou) {
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = i;
                    coup.jFrom = j;
                    coup.iTo = i+k;
                    coup.jTo = j+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup,1);
                            moyenne += meilleurscoup4(partie_copie,joueuria);
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = i+k;
                    coup.jTo = j-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                            appliquer_coup(&partie_copie, coup,1);
                            moyenne += meilleurscoup4(partie_copie,joueuria);
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
            }
        }
        else if (partie.plateau[i][j].p == cavalier) {
            int deplacements[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
            for (int k = 0; k < 8; k++) {
                coup.iFrom = i;
                coup.jFrom = j;
                coup.iTo = i + deplacements[k][0];
                coup.jTo = j + deplacements[k][1];
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
        }
        else if (partie.plateau[i][j].p == roi) {
            for (int k = 0; k < 3; k++) {
                coup.iFrom = i;
                coup.jFrom = j;
                coup.iTo = i + k - 1;
                coup.jTo = j + k - 1;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            // Roque: déplacement du roi de 2 colonnes
            coup.iFrom = i;
            coup.jFrom = j;
            for (int dj = -2; dj <= 2; dj += 4) {
                coup.iTo = i;
                coup.jTo = j + dj;
                if (0 <= coup.jTo && coup.jTo <= 7) {
                    if (mouvement_echecia(coup, partie) && est_mouvement_valide(coup, partie.plateau[i][j].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        partie_copie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
                        appliquer_coup(&partie_copie, coup,1);
                        moyenne += meilleurscoup4(partie_copie,joueuria);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
        }
    }
    }
    }
    if (m == 0){
        return -100;
    }
    return moyenne/m;

}
*/

int meilleurscoup4(Partie partie, int joueuria, Tab* tableau){
    Coup coup;
    Partie partie_copie;
    int moyenne = 0;
    int m = 0;
    for (int i=0; i < 16; i++){
        if (partie.joueur_actif == noir){
            if (tableau[i].p == pion){
                // Teste mouvements d'un pas des pions
                for (int k = 0; k<3;k++){
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.jTo = tableau[i].pos[1]+(-1+k);
                    coup.iTo = (partie.joueur_actif == noir) ? tableau[i].pos[0]+1 : tableau[i].pos[0]-1;
                    if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)){
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
                // Teste mouvements de deux pas des pions
                coup.iFrom = tableau[i].pos[0];
                coup.jFrom = tableau[i].pos[1];
                coup.jTo = tableau[i].pos[1];
                coup.iTo = (partie.joueur_actif == noir) ? tableau[i].pos[0]+2 : tableau[i].pos[0]-2;
                if (coup.iTo >= 0 && coup.iTo < 8 && coup.jTo >= 0 && coup.jTo < 8){
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)){
                        partie_copie = copie_tableau(partie);
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (tableau[i].p == reine) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = k;
                    coup.jTo = tableau[i].pos[1];
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0];
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        moyenne += score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc);
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            
            }
            else if (tableau[i].p == tour) {
                for (int k = 0; k < 8; k++) { // Déplacement en ligne
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = k;
                    coup.jTo = tableau[i].pos[1];
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                        m++;
                        libererTableau(partie_copie);
                    }
                }
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0];
                    coup.jTo = k;
                    if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                        partie_copie = copie_tableau(partie);
                        appliquer_coup(&partie_copie, coup, tableau, 1);
                        moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                        m++;
                        libererTableau(partie_copie);
                    }
                }
            }
            else if (tableau[i].p == fou) {
                for (int k = -7; k <= 7; k++) { // Déplacement en diagonale
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]+k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                    coup.iTo = tableau[i].pos[0]+k;
                    coup.jTo = tableau[i].pos[1]-k;
                    if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
            else if (tableau[i].p == cavalier) {
                int deplacements[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
                for (int k = 0; k < 8; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0] + deplacements[k][0];
                    coup.jTo = tableau[i].pos[1] + deplacements[k][1];
                        if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
            else if (tableau[i].p == roi) {
                for (int k = 0; k < 3; k++) {
                    coup.iFrom = tableau[i].pos[0];
                    coup.jFrom = tableau[i].pos[1];
                    coup.iTo = tableau[i].pos[0] + k - 1;
                    coup.jTo = tableau[i].pos[1] + k - 1;
                        if (0 <=coup.iTo &&  coup.iTo <=7 && 0 <= coup.jTo && coup.jTo <= 7){
                        if (mouvement_echecia(coup, partie, tableau) && est_mouvement_valide(coup, tableau[i].p, partie, partie.joueur_actif)) {
                            partie_copie = copie_tableau(partie);
                            appliquer_coup(&partie_copie, coup, tableau, 1);
                            moyenne += (score(partie_copie,joueuria)-score(partie_copie, (joueuria == blanc) ? noir : blanc));
                            m++;
                            libererTableau(partie_copie);
                        }
                    }
                }
            }
        }
    }
    if (m == 0){
        return -100;
    }
    return moyenne/m;

}