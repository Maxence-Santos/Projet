# Projet Échecs

Projet de jeu d'échecs en C avec intégration du moteur Stockfish.

## Téléchargement et Exécution

### Prérequis
- **Windows** : Les exécutables sont compilés pour Windows uniquement
- **Aucune installation supplémentaire requise** : Tous les runtimes sont inclus statiquement

### Fichiers nécessaires

Pour exécuter le jeu, téléchargez les fichiers suivants depuis GitHub :

1. **`build/Release/chess_game.exe`** - Le jeu d'échecs principal
2. **`build/Release/check_moves.exe`** - Utilitaire d'aide (optionnel)
3. **`stockfish/stockfish-windows-x86-64-avx2.exe`** - Moteur d'échecs Stockfish (requis pour check_moves)

### Instructions

#### Option 1 : Cloner tout le repository
```bash
git clone https://github.com/Maxence-Santos/Projet.git
cd Projet
build\Release\chess_game.exe
```

#### Option 2 : Téléchargement manuel
1. Allez sur https://github.com/Maxence-Santos/Projet
2. Naviguez vers `build/Release/`
3. Téléchargez `chess_game.exe`
4. Double-cliquez pour lancer le jeu

#### Pour utiliser check_moves.exe
Si vous souhaitez utiliser l'utilitaire `check_moves.exe`, vous devez également :
1. Télécharger `stockfish/stockfish-windows-x86-64-avx2.exe`
2. Placer les fichiers dans la structure suivante :
```
votre_dossier/
├── check_moves.exe
└── stockfish/
    └── stockfish-windows-x86-64-avx2.exe
```

## Compilation depuis les sources

Si vous souhaitez recompiler le projet :

### Prérequis
- CMake (version 3.15 ou supérieure)
- Compilateur C/C++ (MSVC, GCC, ou Clang)
- Make ou Visual Studio

### Instructions de compilation

```bash
# Créer le dossier de build
cmake -S . -B build

# Compiler en mode Release
cmake --build build --config Release

# Les exécutables seront dans build/Release/
```

## Structure du projet

- **`Projet_echecs_Rob3-main/`** - Code source du jeu d'échecs en C
- **`stockfish/`** - Code source et exécutable du moteur Stockfish
- **`build/Release/`** - Exécutables compilés
- **`check_moves.cpp`** - Code source de l'utilitaire d'aide

## Notes techniques

- Les exécutables sont compilés avec le **runtime statique** (pas de dépendance aux DLL Visual C++)
- Compatible avec Windows 7 et supérieur
- Taille des fichiers :
  - `chess_game.exe` : ~57 KB
  - `check_moves.exe` : ~144 KB
  - `stockfish-windows-x86-64-avx2.exe` : ~76 MB

## Licence

Voir les fichiers de licence respectifs pour chaque composant.
