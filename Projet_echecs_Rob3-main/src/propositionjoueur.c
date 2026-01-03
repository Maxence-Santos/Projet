#include "../propositionjoueur.h"
#include "../sauvegarde.h"
#include <stdio.h>
#include "../affichage.h"

Coup proposition_joueur(Partie partie) {
    Coup coup;
    char case_from[3], case_to[3];
    int flag = 0;
    char str[2];

    // Demander à l'utilisateur la case de départ
    do {
        printf("Entrez la case de la piece que vous voulez deplacer : ");
        scanf("%2s", case_from);
        str[0] = case_from[0];
        str[1] = case_from[1];
        if ((str[0] >= 'A' && str[0] <= 'H') || (str[0] >= 'a' && str[0] <= 'h')) {
            if (str[1] >= '1' && str[1] <= '8') {
                flag = 1;
            }
            else {
                printf("Case invalide, veuillez recommencer\n");
            }
        } else if ((str[0] == 'q' || str[0] == 's' || str[0] == 'r') && str[1] == '\0') {
            // Commandes à une seule lettre acceptées
            flag = 1;
        } else {
            printf("Case invalide, veuillez recommencer\n");
        }
    } while (flag == 0);

    // Gestion de la commande 's' pour sauvegarder
    if (case_from[0] == 's' && case_from[1] == '\0') {
        sauvegarder_partie(partie);
        return proposition_joueur(partie);  // Re-demander une commande après sauvegarde
    }

    if (case_from[0] == 'r' && case_from[1] == '\0') {
        supprimer_fichier();
        return proposition_joueur(partie);
    }

    // Gestion de la commande 'q' pour quitter
    if (case_from[0] == 'q' && case_from[1] == '\0') {
        char confirmation;
        printf("Voulez-vous sauvegarder avant de quitter ? (o/n) : ");
        scanf(" %c", &confirmation);

        if (confirmation == 'o' || confirmation == 'O') {
            sauvegarder_partie(partie);
            printf("Partie sauvegardee avec succes.\n");
        }
        printf("Vous avez quitte la partie.\n");
        exit(0);  // Quitte le programme
    }

    // Convertir la case source (lettre + chiffre) en indices
    coup.jFrom = case_from[0] - 'A';  // Convertir la lettre en indice de colonne
    coup.iFrom = '8' - case_from[1];  // Convertir le chiffre en indice de ligne

    // Vérifier si les indices sont valides
    if ((coup.iFrom < 0 || coup.iFrom > 7) || (coup.jFrom < 0 || coup.jFrom > 7)) {
        coup.jFrom = case_from[0] - 'a';
        if ((coup.iFrom < 0 || coup.iFrom > 7) || (coup.jFrom < 0 || coup.jFrom > 7)) {
            printf("Case invalide, veuillez recommencer\n");
            return proposition_joueur(partie);  // Demander à nouveau
        }
    }

    // Vérifie que le joueur a bien sélectionné une de ses pièces
    if (partie.joueur_actif != partie.plateau[coup.iFrom][coup.jFrom].c) {
        printf("Ce n'est pas votre piece, veuillez recommencer\n");
        return proposition_joueur(partie);
    }

    flag = 2;
    do {
        // Demander à l'utilisateur la case de destination
        printf("Entrez la case où vous voulez déplacer la pièce : ");
        scanf("%2s", case_to);
        str[0] = case_to[0];
        str[1] = case_to[1];
        if ((str[0] >= 'A' && str[0] <= 'H') || (str[0] >= 'a' && str[0] <= 'h')) {
            if (str[1] >= '1' && str[1] <= '8') {
                flag = 1;
            }
            else {
                printf("Case invalide, veuillez recommencer\n");
            }
        } else if ((str[0] == 'q' || str[0] == 's' || str[0] == 'r') && str[1] == '\0') {
            // Commandes à une seule lettre acceptées
            flag = 1;
        } else {
            printf("Case invalide, veuillez recommencer\n");
        }
    } while (flag == 2);

    // Gestion de la commande 's' pour sauvegarder
    if (case_to[0] == 's' && case_to[1] == '\0') {
        sauvegarder_partie(partie);
        return proposition_joueur(partie);  // Re-demander une commande après sauvegarde
    }

    // Gestion de la commande 'q' pour quitter
    if (case_to[0] == 'q' && case_to[1] == '\0') {
        char confirmation;
        printf("Voulez-vous sauvegarder avant de quitter ? (o/n) : ");
        scanf(" %c", &confirmation);

        if (confirmation == 'o' || confirmation == 'O') {
            sauvegarder_partie(partie);
            printf("Partie sauvegardee avec succes.\n");
        }
        printf("Vous avez quitte la partie.\n");
        exit(0);  // Quitte le programme
    }

    if (case_from[0] == 'r' && case_from[1] == '\0') {
        supprimer_fichier();
        return proposition_joueur(partie);
    }

    // Convertir la case destination (lettre + chiffre) en indices
    coup.jTo = case_to[0] - 'A';  // Convertir la lettre en indice de colonne
    coup.iTo = '8' - case_to[1];  // Convertir le chiffre en indice de ligne

    // Vérifier si les indices sont valides
    if ((coup.iTo < 0 || coup.iTo > 7) || (coup.jTo < 0 || coup.jTo > 7)) {
        coup.jTo = case_to[0] - 'a';
        if ((coup.iTo < 0 || coup.iTo > 7) || (coup.jTo < 0 || coup.jTo > 7)) {
            printf("Case invalide, veuillez recommencer\n");
            return proposition_joueur(partie);  // Demander à nouveau
        }
    }

    // Vérifier si le mouvement est valide
    if (est_mouvement_valide(coup, partie.plateau[coup.iFrom][coup.jFrom].p, partie, partie.joueur_actif)) {
        Partie partie2;
        Tab tableau;
        partie2 = copie_tableau(partie);
        appliquer_coup(&partie2, coup, &tableau, 0);

        // Vérifier si le roi est en échec après le mouvement
        if (est_en_echec(partie2, partie2.joueur_actif)) {
            printf("Mouvement invalide, votre roi serait en echec. Veuillez recommencer.\n");
            libererTableau(partie2);
            return proposition_joueur(partie);  // Re-demander une commande
        }

        libererTableau(partie2);

        // Promotion du pion
        if ((coup.iTo == 0 || coup.iTo == 7) && partie.plateau[coup.iFrom][coup.jFrom].p == pion) {
            printf("\nPromotion du pion !\n");
            printf("Entrez la piece de promotion (Tour, Cavalier, Fou, Reine) : ");
            char promotion[10];
            scanf("%9s", promotion);
            if (strcmp(promotion, "Tour") == 0 || strcmp(promotion, "tour") == 0) {
                partie.plateau[coup.iFrom][coup.jFrom].p = tour;
            } else if (strcmp(promotion, "Cavalier") == 0 || strcmp(promotion, "cavalier") == 0) {
                partie.plateau[coup.iFrom][coup.jFrom].p = cavalier;
            } else if (strcmp(promotion, "Fou") == 0 || strcmp(promotion, "fou") == 0) {
                partie.plateau[coup.iFrom][coup.jFrom].p = fou;
            } else if (strcmp(promotion, "Reine") == 0 || strcmp(promotion, "reine") == 0) {
                partie.plateau[coup.iFrom][coup.jFrom].p = reine;
            } else {
                printf("Piece de promotion invalide, promotion automatique en reine.\n");
                partie.plateau[coup.iFrom][coup.jFrom].p = reine;
            }
        }
        return coup;  // Retourner le coup s'il est valide
    } else {
        printf("Mouvement invalide, veuillez recommencer\n");
        return proposition_joueur(partie);  // Re-demander une commande
    }
}
