cmake_minimum_required(VERSION 3.15)

# Nom du projet et langages
project(projet_echecs LANGUAGES C CXX)

# Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Détection optionnelle de Stockfish
# Permet de passer -DSTOCKFISH_EXECUTABLE=/chemin/stockfish.exe
if(NOT DEFINED STOCKFISH_EXECUTABLE)
    find_program(STOCKFISH_EXECUTABLE
        NAMES stockfish.exe stockfish
        HINTS "${CMAKE_CURRENT_SOURCE_DIR}/stockfish" "${CMAKE_CURRENT_SOURCE_DIR}/stockfish/bin"
        PATHS $ENV{PATH}
    )
endif()

if(NOT STOCKFISH_EXECUTABLE)
    message(STATUS "Stockfish executable not found automatically. You can pass -DSTOCKFISH_EXECUTABLE=/path/to/stockfish.exe to CMake.")
else()
    message(STATUS "Found Stockfish: ${STOCKFISH_EXECUTABLE}")
endif()

# --- Main chess game executable

add_executable(chess_game
    Projet_echecs_Rob3-main/main.c
    Projet_echecs_Rob3-main/src/appliquercoup.c
    Projet_echecs_Rob3-main/src/affichage.c
    Projet_echecs_Rob3-main/src/ia.c
    Projet_echecs_Rob3-main/src/fen.c
    Projet_echecs_Rob3-main/src/copie_tableau.c
    Projet_echecs_Rob3-main/src/est_mouvement_valide.c
    Projet_echecs_Rob3-main/src/creer_plateau.c
    Projet_echecs_Rob3-main/src/mat.c
    Projet_echecs_Rob3-main/src/sauvegarde.c
    Projet_echecs_Rob3-main/src/est_en_echec.c
    Projet_echecs_Rob3-main/src/propositionjoueur.c
    Projet_echecs_Rob3-main/src/score.c
)

target_include_directories(chess_game PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Projet_echecs_Rob3-main"
)

# Lien statique du runtime C pour chess_game (pas de dépendance aux DLL)
if(MSVC)
    set_property(TARGET chess_game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --- Optional: Stockfish integration helper (C++)
add_executable(check_moves
    "check_moves.cpp"
)

# Lien statique du runtime C++ pour check_moves (pas de dépendance aux DLL)
if(MSVC)
    set_property(TARGET check_moves PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(STOCKFISH_EXECUTABLE)
    target_compile_definitions(check_moves PRIVATE STOCKFISH_PATH="${STOCKFISH_EXECUTABLE}")
else()
    target_compile_definitions(check_moves PRIVATE STOCKFISH_PATH="")
endif()

message(STATUS "Build targets:")
message(STATUS "  - chess_game (main game executable)")
message(STATUS "  - check_moves (Stockfish integration helper)")
