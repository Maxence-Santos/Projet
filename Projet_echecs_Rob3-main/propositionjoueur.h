#ifndef PROPOSITION_JOUEUR_H
#define PROPOSITION_JOUEUR_H

#include "struct.h"  // Inclure la définition des structures
#include "est_mouvement_valide.h" // Inclure la fonction de validation du mouvement
#include "copie_tableau.h" // Inclure la fonction de copie du tableau
#include "appliquercoup.h" // Inclure la fonction d'application du coup
#include "est_en_echec.h" // Inclure la fonction de vérification de l'échec

// Déclaration de la fonction proposition_joueur
Coup proposition_joueur(Partie partie);

#endif // PROPOSITION_JOUEUR_H
