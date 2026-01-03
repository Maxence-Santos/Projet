# Nom du compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall

# Répertoire des fichiers sources
SRC_DIR = src

# Liste des fichiers sources (.c)
SRC_FILES = main.c \
            $(SRC_DIR)/affichage.c \
            $(SRC_DIR)/appliquercoup.c \
            $(SRC_DIR)/copie_tableau.c \
            $(SRC_DIR)/creer_plateau.c \
            $(SRC_DIR)/est_en_echec.c \
            $(SRC_DIR)/est_mouvement_valide.c \
            $(SRC_DIR)/mat.c \
            $(SRC_DIR)/propositionjoueur.c \
            $(SRC_DIR)/sauvegarde.c \
            $(SRC_DIR)/ia.c \
            $(SRC_DIR)/score.c

# Liste des fichiers d'en-tête (.h)
INC_FILES = affichage.h \
            appliquercoup.h \
            copie_tableau.h \
            creer_plateau.h \
            est_en_echec.h \
            est_mouvement_valide.h \
            mat.h \
            propositionjoueur.h \
            sauvegarde.h \
            struct.h \
            ia.h \
            score.h

# Nom du fichier exécutable
OUTPUT = main

# Règle par défaut : compilation de l'exécutable
all: $(OUTPUT)

# Règle pour la création de l'exécutable
$(OUTPUT): $(SRC_FILES) $(INC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(OUTPUT)

# Règle pour nettoyer les fichiers générés
clean:
	rm -f $(OUTPUT)

# Règle pour forcer la recompilation
rebuild: clean all
