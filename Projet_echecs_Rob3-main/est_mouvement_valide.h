#ifndef EST_MOUVEMENT_VALIDE_H
#define EST_MOUVEMENT_VALIDE_H

#include <stdbool.h>
#include "struct.h"
#include "appliquercoup.h"
#include "est_en_echec.h"
#include "mat.h"

// Déclaration de la fonction de validation de mouvement
bool est_mouvement_valide(Coup coup, Piece type_piece, Partie partie, Couleur joueur_actif);

bool mouvement_echecia(Coup coup, Partie partie, Tab* tableau);
#endif // MOUVEMENT_H
