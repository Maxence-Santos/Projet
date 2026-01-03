#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "struct.h"
#include <stdbool.h>

#define FICHIER_SAUVEGARDE "sauvegardes.txt"

// Fonction pour sauvegarder une partie
void sauvegarder_partie(Partie partie);

// Fonction pour charger une partie
bool charger_partie(Partie partie, int id, int *donnees);

// Fonction pour lister les parties disponibles
int lister_parties();

// Fonction pour obtenir le prochain identifiant de partie
int prochain_identifiant();

bool supprimer_fichier();

#endif // SAUVEGARDE_H
