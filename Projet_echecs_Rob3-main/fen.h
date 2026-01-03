#ifndef FEN_H
#define FEN_H

#include "struct.h"

// Génère une FEN représentant la position contenue dans `partie`.
// Le côté à jouer est déduit comme étant l'adversaire du joueur_actif
// (on suppose qu'on appelle après l'application d'un coup).
// Retourne une chaîne allouée (malloc) que l'appelant doit free().
char* generate_fen(const Partie* partie);

#endif // FEN_H
