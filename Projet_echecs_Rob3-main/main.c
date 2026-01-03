#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
#endif
#include "creer_plateau.h"
#include "affichage.h"
#include "appliquercoup.h"
#include "propositionjoueur.h"
#include "est_en_echec.h"
#include "mat.h"
#include "sauvegarde.h"
#include <time.h>
#include "ia.h"
#include "fen.h"
#include "copie_tableau.h"

// Global handles pour la communication avec check_moves daemon
#ifdef _WIN32
    static HANDLE g_daemon_stdin = NULL;
    static HANDLE g_daemon_process = NULL;
#else
    static int g_daemon_stdin = -1;
    static pid_t g_daemon_pid = -1;
#endif

// Lancer check_moves daemon quand le jeu est lancé
static int start_daemon(void) {
#ifdef _WIN32
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "check_moves.exe --daemon --quiet");
    
    HANDLE child_in_read = NULL, child_in_write = NULL;
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    
    if (!CreatePipe(&child_in_read, &child_in_write, &sa, 0)) {
        fprintf(stderr, "Failed to create pipe\n");
        return 0;
    }
    
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.hStdInput = child_in_read;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags = STARTF_USESTDHANDLES;
    
    // Création d'une nouvelle console pour le daemon
    if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "Failed to start check_moves daemon\n");
        CloseHandle(child_in_read);
        CloseHandle(child_in_write);
        return 0;
    }

    CloseHandle(child_in_read);  // Fermeture du processus enfant
    g_daemon_stdin = child_in_write;
    g_daemon_process = pi.hProcess;
    CloseHandle(pi.hThread);
    
    return 1;
#else
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        fprintf(stderr, "Failed to create pipe\n");
        return 0;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Failed to fork\n");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return 0;
    }
    
    if (pid == 0) {
        // Processus enfant
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        
        execlp("./check_moves", "check_moves", "--daemon", "--quiet", NULL);
        fprintf(stderr, "Failed to exec check_moves\n");
        exit(1);
    }
    
    // Processus parent
    close(pipe_fd[0]);
    g_daemon_stdin = pipe_fd[1];
    g_daemon_pid = pid;
    
    return 1;
#endif
}

// Query daemon: écriture des FEN et du compt et affichage dans le daemon
static void query_move_count_for_fen(const char *fen, int compt) {
#ifdef _WIN32
    if (!g_daemon_stdin || !fen) return;
    
    DWORD written = 0;
    char buf[1024];
    if (compt >= 0) {
        snprintf(buf, sizeof(buf), "%s|||%d\n", fen, compt);
    } else {
        snprintf(buf, sizeof(buf), "%s\n", fen);
    }
    
    if (!WriteFile(g_daemon_stdin, buf, strlen(buf), &written, NULL)) {
        fprintf(stderr, "Failed to write to daemon\n");
    }
    FlushFileBuffers(g_daemon_stdin);
#else
    if (g_daemon_stdin < 0 || !fen) return;
    
    char buf[1024];
    if (compt >= 0) {
        snprintf(buf, sizeof(buf), "%s|||%d\n", fen, compt);
    } else {
        snprintf(buf, sizeof(buf), "%s\n", fen);
    }
    
    if (write(g_daemon_stdin, buf, strlen(buf)) == -1) {
        fprintf(stderr, "Failed to write to daemon\n");
    }
#endif
}

// Cleanup: stop daemon
static void stop_daemon(void) {
#ifdef _WIN32
    if (g_daemon_stdin) {
        DWORD written = 0;
        WriteFile(g_daemon_stdin, "quit\n", 5, &written, NULL);
        CloseHandle(g_daemon_stdin);
        g_daemon_stdin = NULL;
    }
    if (g_daemon_process) {
        WaitForSingleObject(g_daemon_process, 1000);
        CloseHandle(g_daemon_process);
        g_daemon_process = NULL;
    }
#else
    if (g_daemon_stdin >= 0) {
        write(g_daemon_stdin, "quit\n", 5);
        close(g_daemon_stdin);
        g_daemon_stdin = -1;
    }
    if (g_daemon_pid > 0) {
        waitpid(g_daemon_pid, NULL, 0);
        g_daemon_pid = -1;
    }
#endif
}

Tab *init_tableau() {
    Tab *tableau = malloc(16 * sizeof(Tab));
    if (tableau == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour tableau\n");
        return NULL;
    }

    tableau[0].p = tour; strcpy(tableau[0].pos,"A8"); tableau[1].p = cavalier; strcpy(tableau[1].pos,"B8");
    tableau[2].p = fou; strcpy(tableau[2].pos,"C8"); tableau[3].p = reine; strcpy(tableau[3].pos,"D8");
    tableau[4].p = roi; strcpy(tableau[4].pos,"E8"); tableau[5].p = fou; strcpy(tableau[5].pos,"F8");
    tableau[6].p = cavalier; strcpy(tableau[6].pos,"G8"); tableau[7].p = tour; strcpy(tableau[7].pos,"H8");
    tableau[8].p = pion; strcpy(tableau[8].pos,"A7"); tableau[9].p = pion; strcpy(tableau[9].pos,"B7");
    tableau[10].p = pion; strcpy(tableau[10].pos,"C7"); tableau[11].p = pion; strcpy(tableau[11].pos,"D7");
    tableau[12].p = pion; strcpy(tableau[12].pos,"E7"); tableau[13].p = pion; strcpy(tableau[13].pos,"F7");
    tableau[14].p = pion; strcpy(tableau[14].pos,"G7"); tableau[15].p = pion; strcpy(tableau[15].pos,"H7");

    return tableau;
}

void rejouer(char choixia, Partie partie, Tab *tableau) {
    char r;
    if (choixia == 'o' || choixia == 'O') {
        libererTableau(partie);
        stop_daemon();
        free(tableau);
    }       
    do {
        printf("Voulez-vous rejouer ou quitter ? (r|q) : ");
        scanf("%c", &r);
    } while (r!='r' && r!='q');
    if (r == 'r') main();
    else exit(0);
}

void affiche_mat(char choixia, Partie partie, Tab *tableau) {
    if (est_en_echec(partie, partie.joueur_actif)) {
        printf(" \n");
        if (est_en_mat(partie, partie.joueur_actif)) {
            if (partie.joueur_actif == blanc) {
                printf("\n\nEchec et mat, les noirs gagnent !\n");
            } else {
                printf("\n\nEchec et mat, les blancs gagnent !\n");
            }
            rejouer(choixia, partie, tableau);
        }
        if (partie.joueur_actif == blanc) {
            printf("\nEchec au roi blanc.\n");
        } else {
            printf("\nEchec au roi noir.\n");
        }
    }
}

void charge_partie(Partie *partie) {
    int nb_parties_save = lister_parties();
    if (nb_parties_save == 0) {
        printf("Aucune partie sauvegardée trouvée. Une nouvelle partie sera creee.\n");
        printf("Combien de temps voulez-vous que la partie dure ? (en secondes) : ");
        scanf("%d", &partie->temps_blanc);
        partie->temps_noir = partie->temps_blanc;
    }else {
        printf("Entrez l'identifiant de la partie à charger : ");
        int id;
        scanf("%d", &id);
        int tab[4];
        if (charger_partie(*partie, id, tab)) {
            partie->joueur_actif = tab[0];
            partie->temps_blanc = tab[1];
            partie->temps_noir = tab[2];
            partie->nb_coups = tab[3];
            partie->joueur_actif = (partie->joueur_actif == blanc) ? noir : blanc;
            printf("Partie chargee avec succes.\n");
        } else {
            printf("Aucune partie sauvegardee trouvee. Une nouvelle partie sera creee.\n");
            printf("Combien de temps voulez-vous que la partie dure ? (en secondes) : ");
            scanf("%d", &partie->temps_blanc);
            partie->temps_noir = partie->temps_blanc;
        }
    }
}

void mouvement_ia(Partie partie, Tab *tableau) {
    if (partie.joueur_actif == blanc) {
        appliquer_coup(&partie, proposition_joueur(partie), tableau, 0);
    } else {
        char *fen = generate_fen(&partie);
        Coup ia_move = proposition_ia(partie, tableau);
        query_move_count_for_fen(fen, g_last_compt);
        free(fen);
        appliquer_coup(&partie, ia_move, tableau, 1);
    }
}

int main() {
    // Afficher au format UTF-8 sur la console Windows pour les pièces d'échecs Unicode
    #ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    #endif
    
    Partie partie;
    char choix;
    char choixia;

    partie.nb_coups = 0;
    printf("Bienvenue dans le jeu d'echecs.\n");

    do {
        printf("Voulez-vous jouer contre une IA ? (o/n) : ");
        scanf(" %c", &choixia);
    } while (choixia != 'o' && choixia != 'O' && choixia != 'n' && choixia != 'N');

    do {
        printf("Voulez-vous charger une partie ? (o/n) : ");
        scanf(" %c", &choix);
    } while (choix != 'o' && choix != 'O' && choix != 'n' && choix != 'N');
    
    partie.plateau = creerplateau();
    partie.joueur_actif = noir; //passera à blanc au début du jeu
    init_partie(&partie);
    Tab *tableau = init_tableau();

    if (choix == 'o' || choix == 'O') {
        charge_partie(&partie);
    } else {
        printf("Combien de temps voulez-vous que la partie dure ? (en secondes) : ");
        scanf("%d", &partie.temps_blanc);
        partie.temps_noir = partie.temps_blanc;
    }

    if (choixia == 'o' || choixia == 'O') {
        // Lance check_moves dans une nouvelle console
        if (!start_daemon()) {
            fprintf(stderr, "Failed to start daemon\n");
            return 1;
        }
        #ifdef _WIN32
            Sleep(500);
        #else
            usleep(500000);  // 500ms = 500000 microseconds
        #endif
    }
    while (1) {
        partie.joueur_actif = (partie.joueur_actif == blanc) ? noir : blanc;
        affichage(partie);

        affiche_mat(choixia, partie, tableau);

        int avant = time(NULL);
        if (choixia == 'o' || choixia == 'O') {
            mouvement_ia(partie, tableau);
        }
        else {
            appliquer_coup(&partie, proposition_joueur(partie), tableau, 0);
        }
        partie.nb_coups++;
        int apres = time(NULL);
        if (partie.joueur_actif == blanc) {
            partie.temps_blanc -= apres - avant;
        } else {
            partie.temps_noir -= apres - avant;
        }

        if (partie.temps_blanc <= 0) {
            printf("Temps ecoule pour les blancs, les noirs gagnent !\n");     
            rejouer(choixia, partie, tableau);
        }
        if (partie.temps_noir <= 0) {
            printf("Temps ecoule pour les noirs, les blancs gagnent !\n");
            rejouer(choixia, partie, tableau);
        }
    }
    return 0;
}