// main.c — Menus et boucle principale de CY-biblioTECH
// Interface terminal style retro avec navigation au clavier

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/utilisateurs.h"
#include "../include/livres.h"

// ─────────────────────────────────────────────
//  EFFACEMENT DE L'ECRAN
// ─────────────────────────────────────────────
#define CLEAR "\033[2J\033[H"

// ─────────────────────────────────────────────
//  DIMENSIONS
// ─────────────────────────────────────────────
#define LARGEUR 52

// ─────────────────────────────────────────────
//  DONNEES UTILISATEURS (chargees au demarrage)
// ─────────────────────────────────────────────
static Utilisateur  users[MAX_UTILISATEURS];  // tableau de tous les utilisateurs
static int          nb_users = 0;             // nombre d'utilisateurs charges

// ─────────────────────────────────────────────
//  ETAT DE SESSION
// ─────────────────────────────────────────────
static int         connecte       = 0;
static Utilisateur user_actuel;               // utilisateur actuellement connecte

// ─────────────────────────────────────────────
//  HELPERS D'AFFICHAGE
// ─────────────────────────────────────────────

static void afficher_bandeau(const char *titre) {
    int titre_len = (int)strlen(titre);
    int hachures  = (LARGEUR - titre_len - 2) / 2;

    for (int i = 0; i < hachures; i++) printf("#");
    if (titre_len > 0) printf(" %s ", titre);
    for (int i = 0; i < hachures; i++) printf("#");
    if (titre_len > 0 && (hachures * 2 + titre_len + 2) < LARGEUR) printf("#");
    printf("\n");
}

static void afficher_option(const char *touche, const char *label, int disponible) {
    if (disponible) {
        printf("  %s. %s\n", touche, label);
    } else {
        printf("  %s. %s  (prof uniquement)\n", touche, label);
    }
}

static char lire_touche(void) {
    char buf[8];
    printf("\n  Entrez votre choix : ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) return '0';
    return buf[0];
}

static void pause_entree(void) {
    printf("  (Appuyez sur Entree pour continuer)");
    getchar();
}

// ─────────────────────────────────────────────
//  MENU DE CONNEXION
// ─────────────────────────────────────────────
static void menu_connexion(void) {
    printf(CLEAR);
    afficher_bandeau("CY-biblioTECH");
    printf("\n");
    afficher_option("1", "Se connecter",           1);
    afficher_option("2", "Creer un nouveau compte", 1);
    afficher_option("0", "Quitter",                1);
    printf("\n");
    afficher_bandeau("");

    char c = lire_touche();

    switch (c) {
        case '1': {
            // Saisie du login
            char login_saisi[TAILLE_LOGIN];
            char mdp_saisi[TAILLE_MDP];

            printf("\n  Login      : ");
            fgets(login_saisi, sizeof(login_saisi), stdin);
            login_saisi[strcspn(login_saisi, "\n")] = '\0';

            printf("  Mot de passe : ");
            fgets(mdp_saisi, sizeof(mdp_saisi), stdin);
            mdp_saisi[strcspn(mdp_saisi, "\n")] = '\0';

            // Appel de la fonction login() depuis utilisateurs.c
            if (login(users, nb_users, login_saisi, mdp_saisi, &user_actuel)) {
                connecte = 1;
                printf("\n  Bienvenue, %s ! (%s)\n",
                       user_actuel.login,
                       user_actuel.role == PROFESSEUR ? "Professeur" : "Etudiant");
            } else {
                printf("\n  Login ou mot de passe incorrect.\n");
            }
            pause_entree();
            break;
        }

        case '2': {
            // Saisie des informations du nouveau compte
            char login_saisi[TAILLE_LOGIN];
            char mdp_saisi[TAILLE_MDP];
            char role_saisi[4];

            printf("\n  Choisissez un login      : ");
            fgets(login_saisi, sizeof(login_saisi), stdin);
            login_saisi[strcspn(login_saisi, "\n")] = '\0';

            printf("  Choisissez un mot de passe : ");
            fgets(mdp_saisi, sizeof(mdp_saisi), stdin);
            mdp_saisi[strcspn(mdp_saisi, "\n")] = '\0';

            printf("  Role (0 = Etudiant, 1 = Professeur) : ");
            fgets(role_saisi, sizeof(role_saisi), stdin);
            Role role = (role_saisi[0] == '1') ? PROFESSEUR : ETUDIANT;

            // Appel de la fonction creer_compte() depuis utilisateurs.c
            int resultat = creer_compte(users, &nb_users,
                                        login_saisi, mdp_saisi, role);
            if (resultat == 1) {
                printf("\n  Compte cree avec succes ! Vous pouvez maintenant vous connecter.\n");
            } else if (resultat == 0) {
                printf("\n  Ce login est deja utilise, choisissez-en un autre.\n");
            } else {
                printf("\n  Erreur lors de la creation du compte.\n");
            }
            pause_entree();
            break;
        }

        case '0':
            printf(CLEAR "  Au revoir !\n\n");
            exit(0);

        default:
            printf("\n  Choix invalide.\n");
            pause_entree();
            break;
    }
}

// ─────────────────────────────────────────────
//  MENU PRINCIPAL (apres connexion)
// ─────────────────────────────────────────────
static void menu_principal(Livre biblio[], int *nb_livres) {
    printf(CLEAR);
    afficher_bandeau("MENU PRINCIPAL");
    printf("\n");

    printf("  Connecte en tant que : %s (%s)\n\n",
           user_actuel.login,
           user_actuel.role == PROFESSEUR ? "Professeur" : "Etudiant");

    afficher_option("1", "Emprunter un livre",          1);
    afficher_option("2", "Rendre un livre",             1);
    afficher_option("3", "Mes emprunts en cours",       1);
    afficher_option("4", "Rechercher un livre",         1);
    afficher_option("5", "Liste de tous les livres",    1);
    afficher_option("6", "Ajouter un livre au catalogue",
                    user_actuel.role == PROFESSEUR);
    afficher_option("7", "Voir mes retards",            1);
    afficher_option("9", "Se deconnecter",              1);
    afficher_option("0", "Quitter l'application",       1);
    printf("\n");
    afficher_bandeau("");

    char c = lire_touche();

    switch (c) {
        case '1':
            // TODO : appeler emprunter_livre() depuis emprunts.c
            printf("\n  [TODO] Emprunter un livre\n");
            pause_entree();
            break;

        case '2':
            // TODO : appeler rendre_livre() depuis emprunts.c
            printf("\n  [TODO] Rendre un livre\n");
            pause_entree();
            break;

        case '3':
            // TODO : appeler afficher_emprunts_utilisateur() depuis emprunts.c
            printf("\n  [TODO] Affichage de vos emprunts en cours\n");
            pause_entree();
            break;

        case '4':
            rechercher_livre(biblio, nb_livres);
            printf("\n  [TODO] Rechercher un livre (titre / auteur / categorie)\n");
            pause_entree();
            break;

        case '5':
            afficher_les_livres(biblio, nb_livres);
            printf("\n  [TODO] Liste complete des livres\n");
            pause_entree();
            break;

        case '6':
            if (user_actuel.role == PROFESSEUR) {
                ajouter_livre(biblio, &nb_livres);
                printf("\n  [TODO] Ajouter un livre au catalogue\n");
            } else {
                printf("\n  Acces reserve aux professeurs.\n");
            }
            pause_entree();
            break;

        case '7':
            // TODO : appeler afficher_retards() depuis emprunts.c
            printf("\n  [TODO] Vos livres en retard\n");
            pause_entree();
            break;

        case '9':
            connecte = 0;
            memset(&user_actuel, 0, sizeof(user_actuel));
            break;

        case '0':
            printf(CLEAR "  Au revoir !\n\n");
            exit(0);

        default:
            printf("\n  Choix invalide.\n");
            pause_entree();
            break;
    }
}

// ─────────────────────────────────────────────
//  POINT D'ENTREE
// ─────────────────────────────────────────────
int main(void) {

    Livre biblio[200];
    int nb_livres = 0;

    nb_users = charger_utilisateurs(users, MAX_UTILISATEURS);
    charger_livres(biblio, &nb_livres);

    if (nb_users == -1) {
        printf("  Erreur : impossible de charger les utilisateurs.\n");
        return 1;
    }

    // TODO : charger_livres(biblio, &nb_livres);

    while (1) {
        if (!connecte)
            menu_connexion();
        else    
            menu_principal(biblio, &nb_livres);
    }

    return 0;
}
