#ifndef EST_EN_ECHEC_H
#define EST_EN_ECHEC_H

#include <stdbool.h>
#include "struct.h"
#include <stdio.h>
#include <math.h>
#include "est_mouvement_valide.h"

// Déclaration de la fonction de validation de mouvement
bool est_en_echec(Partie partie2, Couleur joueur_actif);

#endif 
