// main.c — Menus et boucle principale de CY-biblioTECH
// Interface terminal style retro avec navigation au clavier

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLEAR "\033[2J\033[H"
#define LARGEUR 52

// ─────────────────────────────────────────────
//  ETAT DE SESSION (a relier aux autres modules)
// ─────────────────────────────────────────────
static int  connecte         = 0;
static char login_actuel[64] = "";
static int  est_professeur   = 0;


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
        case '1':
            // TODO : appeler login() depuis utilisateurs.c
            printf("\n  Login : ");
            fgets(login_actuel, sizeof(login_actuel), stdin);
            login_actuel[strcspn(login_actuel, "\n")] = '\0';
            connecte       = 1;
            est_professeur = 0; // TODO : lire le role depuis le fichier
            printf("\n  Bienvenue, %s !\n", login_actuel);
            pause_entree();
            break;

        case '2':
            // TODO : appeler creer_compte() depuis utilisateurs.c
            printf("\n  [TODO] Creation de compte\n");
            pause_entree();
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
//  MENU PRINCIPAL (apres connexion)
// ─────────────────────────────────────────────
static void menu_principal(void) {
    printf(CLEAR);
    afficher_bandeau("MENU PRINCIPAL");
    printf("\n");

    printf("  Connecte en tant que : %s (%s)\n\n",
           login_actuel,
           est_professeur ? "Professeur" : "Etudiant");

    afficher_option("1", "Emprunter un livre",          1);
    afficher_option("2", "Rendre un livre",             1);
    afficher_option("3", "Mes emprunts en cours",       1);
    afficher_option("4", "Rechercher un livre",         1);
    afficher_option("5", "Liste de tous les livres",    1);
    afficher_option("6", "Ajouter un livre au catalogue", est_professeur);
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
            // TODO : appeler rechercher_livre() depuis livres.c
            printf("\n  [TODO] Rechercher un livre (titre / auteur / categorie)\n");
            pause_entree();
            break;

        case '5':
            // TODO : appeler afficher_les_livres() depuis livres.c
            printf("\n  [TODO] Liste complete des livres\n");
            pause_entree();
            break;

        case '6':
            if (est_professeur) {
                // TODO : appeler ajouter_livre() depuis livres.c
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
            memset(login_actuel, 0, sizeof(login_actuel));
            est_professeur = 0;
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
    // TODO : charger_livres(biblio, &nbLivres);
    // TODO : charger_utilisateurs(users, &nbUsers);

    while (1) {
        if (!connecte)
            menu_connexion();
        else
            menu_principal();
    }

    return 0;
}
