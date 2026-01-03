#ifndef IA_H
#define IA_H

#include "struct.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "score.h"
#include "est_mouvement_valide.h"
#include "copie_tableau.h"
#include "appliquercoup.h"
#include "affichage.h"
#include <time.h>

// Global variable to track legal move count from last proposition_ia call
extern int g_last_compt;

Coup proposition_ia(Partie partie, Tab *tableau);

int meilleurscoup1(Partie partie, int joueuria, Tab *tableau);

int meilleurscoup2(Partie partie, int joueuria, Tab* tableau);

//int meilleurscoup3(Partie partie, int joueuria); POSSIBLE MAIS TROP DE CALCUL

int meilleurscoup4(Partie partie, int joueuria, Tab* tableau);

#endif
