#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../sauvegarde.h"

void sauvegarder_partie(Partie partie) {
    FILE* fichier = fopen(FICHIER_SAUVEGARDE, "a");
    if (!fichier) {
        printf("Erreur : Impossible d'ouvrir le fichier de sauvegarde.\n");
        return;
    }

    int id = prochain_identifiant();
    fprintf(fichier, "# Partie %d\n", id);
    fprintf(fichier, "# Joueur %d, Temps restant B : %d, N : %d, Nb de coup %d\n", partie.joueur_actif, partie.temps_blanc, partie.temps_noir, partie.nb_coups);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            fprintf(fichier, "%d %d ", partie.plateau[i][j].p, partie.plateau[i][j].c);
        }
        fprintf(fichier, "\n");
    }
    fprintf(fichier, "\n");

    fclose(fichier);
    printf("Partie sauvegardee sous l'identifiant %d.\n", id);
}

bool charger_partie(Partie partie, int id, int *donnees) {
    FILE *file = fopen(FICHIER_SAUVEGARDE, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier de sauvegarde");
        return false;
    }
     // Nous chargeons simplement la première partie sauvegardée
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        int partie_id;
        if (sscanf(buffer, "# Partie %d", &partie_id) == 1 && partie_id == id) {
            printf("Chargement de la partie %d...\n", id);
            // Charger les données de la partie ici (plateau, joueur_actif)

            int a1, a2, a3, a4; 
            fscanf(file, "# Joueur %d, Temps restant B : %d, N : %d, Nb de coup %d", &a1, &a2, &a3, &a4);
            donnees[0] = a1;
            donnees[1] = a2;
            donnees[2] = a3;
            donnees[3] = a4;

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    int piece, couleur;
                    fscanf(file, "%d %d", &piece, &couleur);
                    partie.plateau[i][j].p = piece;
                    partie.plateau[i][j].c = couleur;
                }
            }
            fclose(file);
            return true; // Chargement réussi
        }
    }

    fclose(file);
    return false; // Aucune partie trouvée
}

int lister_parties() {
    int parties = 0;
    FILE* fichier = fopen(FICHIER_SAUVEGARDE, "r");
    if (fichier == NULL) {
        printf("Aucune sauvegarde disponible.\n");
        return parties;
    }

    char ligne[256];
    printf("Parties disponibles :\n");
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strncmp(ligne, "# Partie", 8) == 0) {
            printf("%s", ligne);
            parties++;
        }
    }

    fclose(fichier);
    return parties;
}

int prochain_identifiant() {
    FILE* fichier = fopen(FICHIER_SAUVEGARDE, "r");
    if (fichier == NULL) {
        return 1; // Première partie
    }

    char ligne[256];
    int dernier_id = 0;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strncmp(ligne, "# Partie", 8) == 0) {
            sscanf(ligne, "# Partie %d", &dernier_id);
        }
    }

    fclose(fichier);
    return dernier_id + 1;
}

bool supprimer_fichier() {
    if (remove(FICHIER_SAUVEGARDE) == 0) {
        printf("Les sauvegardes ont ete supprime avec succes.\n");
        return true;
    } else {
        perror("Erreur lors de la suppression du fichier");
        return false;
    }
}
