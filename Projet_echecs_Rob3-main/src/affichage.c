#include "../affichage.h"

// Fonction pour convertir une pièce en un caractère, en tenant compte de sa couleur.
void afficher_piece(Piece p, Couleur c) {
    switch (p) {
        case tour:
            if (c != blanc) {
                printf("♖ ");
            } else {
                printf("♜ ");
            }
        break;
        case cavalier:
        if (c != blanc) {
                printf("♘ ");
            } else {
                printf("♞ ");
            }
        break;
        case fou:
        if (c != blanc) {
                printf("♗ ");
            } else {
                printf("♝ ");
            }
        break;
        case reine:
        if (c != blanc) {
                printf("♕ ");
            } else {
                printf("♛ ");
            }
        break;
        case roi:
        if (c != blanc) {
                printf("♔ ");
            } else {
                printf("♚ ");
            }
        break;
        case pion:
        if (c != blanc) {
                printf("♙ ");
            } else {
                printf("♟ ");
            }
        break;
        case vide:
            printf("  ");
        break;
    }
}

// Fonction pour afficher l'état du jeu à l'écran (sortie console).
void affichage(Partie partie) {
    Case** grille = partie.plateau;
    if (partie.joueur_actif == blanc){
        printf("\n\n  A  B  C  D  E  F  G  H             AU TOUR DES BLANCS DE JOUER\n ");
    }
    else{
        printf("\n\n  A  B  C  D  E  F  G  H             AU TOUR DES NOIRS DE JOUER\n ");
    }
    for (int i = 0; i < 8; i++) {
        printf("┼──");
    }
    printf("┼\n");
    for (int i = 0; i < 8; i++) {
        printf("%d", 8 - i);
        for (int j = 0; j < 8; j++) {
            printf("│");
            afficher_piece(grille[i][j].p, grille[i][j].c);
        }
        if (i == 7){
            printf("│1  Score blanc: %d et temps restant : %d secondes\n", score(partie, blanc) - score(partie, noir), partie.temps_blanc);
        }
        else if (i == 0){
            printf("│8  Score noir: %d et temps restant : %d secondes\n", score(partie, noir) - score(partie, blanc), partie.temps_noir);
        }
        else if (i == 2){
            printf("│6  Exemple de format pour les cases : 'e2' ou 'F5'\n");
        }

        else if (i == 4){
            printf("│4    's' pour sauvegarder, 'q' pour quitter\n");
        }
        else if (i == 5){
            printf("│3                'r' pour reset\n");
        }
        else{
        printf("│%d\n", 8 - i);
        }
        printf(" ");
        for (int i = 0; i < 8; i++) {
            printf("┼──");
        }
        printf("┼\n");
    }
    printf("  A  B  C  D  E  F  G  H\n\n\n");
}