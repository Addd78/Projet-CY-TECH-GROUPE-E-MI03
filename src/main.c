// main.c — Point d'entrée et gestion des menus de CY-biblioTECH
// Contient : affichage, saisie sécurisée MDP, boucle principale
// Les fonctions métier (livres, emprunts, users) sont dans leurs fichiers respectifs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>     // _getch() : lit une touche sans l'afficher (spécifique Windows/MinGW)
#include "../include/utilisateurs.h"
#include "../include/livres.h"
#include "../include/emprunts.h"

// --- SÉQUENCES ANSI ---
// \033 = caractère Échap, [2J = efface l'écran, [H = curseur en haut à gauche
#define CLEAR   "\033[2J\033[H"
#define LARGEUR 50   // largeur des bandeaux en nombre de caractères

// Codes couleur ANSI — fonctionnent dans CMD si le terminal les supporte
#define VERT    "\033[32m"   // vert normal (encadrements)
#define VVERT   "\033[92m"   // vert vif (options actives)
#define JAUNE   "\033[93m"   // jaune vif (touches, titres)
#define CYAN    "\033[96m"   // cyan vif (invites de saisie, bandeaux)
#define ROUGE   "\033[91m"   // rouge vif (erreurs)
#define BLANC   "\033[97m"   // blanc vif (noms d'utilisateur)
#define GRIS    "\033[90m"   // gris sombre (infos secondaires, options désactivées)
#define RESET   "\033[0m"    // remet la couleur par défaut du terminal
#define BOLD    "\033[1m"    // texte en gras

// --- VARIABLES GLOBALES ---
// Tableau de tous les comptes chargés depuis le fichier au démarrage
static Utilisateur users[MAX_UTILISATEURS];
static int nb_users = 0;       // nombre d'utilisateurs effectivement chargés

// État de la session courante : 0 = non connecté, 1 = connecté
static int connecte = 0;
static Utilisateur user_actuel; // données de l'utilisateur actuellement connecté

// --- SAISIE SÉCURISÉE DU MOT DE PASSE ---
// Affiche '*' à la place des caractères tapés
// TAB : bascule entre mode masqué et mode visible (réaffiche toute la saisie)
// Backspace : efface le dernier caractère (visuellement + dans le buffer)
// Entrée : valide et termine la saisie
static void saisir_mdp(char *buf, int taille) {
    int visible = 0;   // 0 = masqué par des '*', 1 = caractères en clair
    int len = 0;       // nombre de caractères saisis jusqu'ici
    printf(GRIS " [TAB = afficher/masquer]" RESET " ");
    memset(buf, 0, taille);   // on part d'un buffer vide
    while (1) {
        int c = _getch();   // lit une touche sans l'afficher dans le terminal
        if (c == '\r' || c == '\n') {
            // Entrée : fin de saisie
            printf("\n");
            break;
        } else if (c == '\t') {
            // TAB : bascule visible/masqué
            visible = !visible;
            // On calcule la largeur totale de la ligne affichée pour l'effacer entièrement
            // 26 = longueur de " [TAB = afficher/masquer] "
            int largeur = 26 + len;
            printf("\r");                                   // retour début de ligne
            for (int i = 0; i < largeur; i++) printf(" "); // écrase tout avec des espaces
            printf("\r");                                   // retour début de ligne à nouveau
            // Réaffiche le label + les caractères dans le nouveau mode
            printf(GRIS " [TAB = afficher/masquer]" RESET " ");
            for (int i = 0; i < len; i++) printf("%c", visible ? buf[i] : '*');
            fflush(stdout);   // force l'affichage immédiat (CMD bufferise parfois)
        } else if (c == '\b' || c == 127) {
            // Backspace : supprime le dernier caractère
            if (len > 0) {
                len--;
                buf[len] = '\0';
                printf("\b \b");   // recule, écrase le '*' affiché, recule encore
            }
        } else if (c >= 32 && c < 127 && len < taille - 1) {
            // Caractère imprimable normal : on le stocke dans le buffer
            buf[len++] = (char)c;
            // On affiche soit le vrai caractère soit un '*' selon le mode
            printf("%c", visible ? (char)c : '*');
        }
    }
    buf[len] = '\0';   // termine proprement la chaîne
}

// --- AFFICHAGE D'UN BANDEAU ---
// Centre le titre entre des '#' pour atteindre LARGEUR caractères au total
// Si titre est vide, affiche une ligne de '#' simple (séparateur)
static void afficher_bandeau(const char *titre) {
    int titre_len = (int)strlen(titre);
    int hachures  = (LARGEUR - titre_len - 2) / 2;   // -2 pour les espaces autour du titre
    printf(CYAN);
    for (int i = 0; i < hachures; i++) printf("#");
    if (titre_len > 0) printf(BOLD BLANC " %s " RESET CYAN, titre);
    for (int i = 0; i < hachures; i++) printf("#");
    // Ajoute un '#' final si la longueur totale est impaire (pour garder LARGEUR)
    if (titre_len > 0 && (hachures * 2 + titre_len + 2) < LARGEUR) printf("#");
    printf(RESET "\n");
}

// --- AFFICHAGE D'UNE OPTION DU MENU ---
// disponible = 1 : option en couleur normale
// disponible = 0 : option grisée avec mention "(prof uniquement)"
static void afficher_option(const char *touche, const char *icone, const char *label, int disponible) {
    if (disponible) {
        printf(JAUNE "  %s" RESET ". " VVERT "%s %s\n" RESET, touche, icone, label);
    } else {
        // Option réservée aux profs : affichée en gris pour montrer qu'elle existe mais est verrouillée
        printf(GRIS "  %s. %s %s  (prof uniquement)\n" RESET, touche, icone, label);
    }
}

// --- LECTURE D'UNE TOUCHE AU MENU ---
// Utilise fgets (pas _getch) car ici on veut que l'utilisateur voie ce qu'il tape
static char lire_touche(void) {
    char buf[8];
    printf(CYAN "\nEntrez votre choix : " RESET);
    if (fgets(buf, sizeof(buf), stdin) == NULL) return '0';
    return buf[0];   // on ne garde que le premier caractère saisi
}

// --- PAUSE ENTRE DEUX ÉCRANS ---
// Attend que l'utilisateur appuie sur Entrée avant de revenir au menu
static void pause_entree(void) {
    printf(GRIS "(Appuyez sur Entree pour continuer)" RESET);
    int m;
    while ((m = getchar()) != '\n' && m != EOF);   // vide le buffer jusqu'au '\n'
}

// --- MENU DE CONNEXION (écran d'accueil) ---
// Affiché tant que personne n'est connecté
// Gère : connexion, création de compte, quitter
static void menu_connexion(void) {
    printf(CLEAR);
    // Encadrement ASCII retro — compatible tous terminaux (pas d'UTF-8)
    printf(VERT  "+==================================================+\n" RESET);
    printf(VERT  "|" JAUNE BOLD "   [~]  Bienvenue sur CY-biblioTECH  [~]" RESET VERT "          |\n" RESET);
    printf(VERT  "|" GRIS  "        Votre bibliotheque numerique              " VERT "|\n" RESET);
    printf(VERT  "|" CYAN  "   [*]  [*]  [*]  [*]  [*]  [*]  [*]  [*]  [*]   " VERT " |\n" RESET);
    printf(VERT  "+==================================================+\n" RESET);
    printf("\n");
    afficher_bandeau("CY-biblioTECH");
    printf("\n");
    afficher_option("1", "[>]", "Se connecter", 1);
    afficher_option("2", "[+]", "Creer un nouveau compte", 1);
    afficher_option("0", "[X]", "Quitter", 1);
    printf("\n");
    afficher_bandeau("");   // séparateur bas
    char c = lire_touche();
    switch (c) {
        case '1': {
            // --- Connexion ---
            char login_saisi[TAILLE_LOGIN];
            char mdp_saisi[TAILLE_MDP];
            printf(CYAN "\nLogin : " RESET);
            fgets(login_saisi, sizeof(login_saisi), stdin);
            login_saisi[strcspn(login_saisi, "\n")] = '\0';   // retire le '\n' de fgets
            printf(CYAN "Mot de passe : " RESET);
            saisir_mdp(mdp_saisi, TAILLE_MDP);   // saisie avec masquage par '*'
            // Vérifie les identifiants dans le tableau users chargé au démarrage
            if (login(users, nb_users, login_saisi, mdp_saisi, &user_actuel)) {
                connecte = 1;   // passe en mode "connecté" → boucle principale ira sur menu_principal
                printf(VVERT "\n[OK] Bienvenue, %s ! (%s)\n" RESET,
                       user_actuel.login,
                       user_actuel.role == PROFESSEUR ? "Professeur" : "Etudiant");
            } else {
                printf(ROUGE "\n[!!] Login ou mot de passe incorrect.\n" RESET);
            }
            pause_entree();
            break;
        }
        case '2': {
            // --- Création de compte ---
            char login_saisi[TAILLE_LOGIN];
            char mdp_saisi[TAILLE_MDP];
            char role_saisi[4];
            printf(CYAN "\nChoisissez un login      : " RESET);
            fgets(login_saisi, sizeof(login_saisi), stdin);
            login_saisi[strcspn(login_saisi, "\n")] = '\0';
            printf(CYAN "Choisissez un mot de passe : " RESET);
            saisir_mdp(mdp_saisi, TAILLE_MDP);   // saisie avec masquage par '*'
            printf(CYAN "Role (0 = Etudiant, 1 = Professeur) : " RESET);
            fgets(role_saisi, sizeof(role_saisi), stdin);
            Role role = (role_saisi[0] == '1') ? PROFESSEUR : ETUDIANT;
            // creer_compte retourne : 1 = succès, 0 = login déjà pris, -1 = erreur
            int resultat = creer_compte(users, &nb_users, login_saisi, mdp_saisi, role);
            if (resultat == 1) {
                printf(VVERT "\n[OK] Compte cree ! Vous pouvez maintenant vous connecter.\n" RESET);
            } else if (resultat == 0) {
                printf(ROUGE "\n[!!] Ce login est deja utilise, choisissez-en un autre.\n" RESET);
            } else {
                printf(ROUGE "\n[!!] Erreur lors de la creation du compte.\n" RESET);
            }
            pause_entree();
            break;
        }
        case '0':
            printf(CLEAR JAUNE "Au revoir !\n\n" RESET);
            exit(0);
        default:
            printf(ROUGE "\n[!!] Choix invalide.\n" RESET);
            pause_entree();
            break;
    }
}

// --- MENU PRINCIPAL (après connexion) ---
// Affiché tant que l'utilisateur est connecté
// Les options prof (ajouter un livre) sont grisées pour les étudiants
static void menu_principal(Livre biblio[], int *nb_livres) {
    printf(CLEAR);
    afficher_bandeau("MENU PRINCIPAL");
    printf("\n");
    // Rappel de qui est connecté et de son rôle
    printf(GRIS "  Connecte : " RESET BOLD BLANC "%s" RESET GRIS " (%s)\n\n" RESET,
           user_actuel.login,
           user_actuel.role == PROFESSEUR ? "Professeur" : "Etudiant");
    // Icones ASCII choisies pour correspondre visuellement à chaque action
    afficher_option("1", "[>>]", "Emprunter un livre", 1);        // >> = prendre
    afficher_option("2", "[<<]", "Rendre un livre", 1);           // << = rapporter
    afficher_option("3", "[=]",  "Mes emprunts en cours", 1);     // = = liste/tableau
    afficher_option("4", "[?]",  "Rechercher un livre", 1);       // ? = question/recherche
    afficher_option("5", "[#]",  "Liste de tous les livres", 1);  // # = catalogue complet
    afficher_option("6", "[+]",  "Ajouter un livre au catalogue", user_actuel.role == PROFESSEUR);  // réservé prof
    afficher_option("7", "[!]",  "Voir mes retards", 1);          // ! = alerte
    afficher_option("9", "[<]",  "Se deconnecter", 1);            // < = retour en arrière
    afficher_option("0", "[X]",  "Quitter l'application", 1);     // X = fermer
    printf("\n");
    afficher_bandeau("");   // séparateur bas
    char c = lire_touche();
    switch (c) {
        case '1':
            emprunter_livre(biblio, *nb_livres, &user_actuel);
            pause_entree();
            break;
        case '2':
            rendre_livre(biblio, *nb_livres, &user_actuel);
            pause_entree();
            break;
        case '3':
            afficher_emprunts_utilisateur(biblio, *nb_livres, &user_actuel);
            pause_entree();
            break;
        case '4':
            rechercher_livre(biblio, *nb_livres);
            pause_entree();
            break;
        case '5':
            afficher_les_livres(biblio, *nb_livres);
            pause_entree();
            break;
        case '6':
            // Double vérification du rôle (la 1ère est visuelle via afficher_option)
            if (user_actuel.role == PROFESSEUR) {
                ajouter_livre(biblio, nb_livres);
            } else {
                printf(ROUGE "\n[!!] Acces reserve aux professeurs.\n" RESET);
            }
            pause_entree();
            break;
        case '7':
            afficher_retards(biblio, *nb_livres, &user_actuel);
            pause_entree();
            break;
        case '9':
            // Déconnexion : remet connecte à 0 et efface les données de session
            connecte = 0;
            memset(&user_actuel, 0, sizeof(user_actuel));   // efface les données de l'utilisateur courant
            break;
        case '0':
            printf(CLEAR JAUNE "Au revoir !\n\n" RESET);
            exit(0);
        default:
            printf(ROUGE "\n[!!] Choix invalide.\n" RESET);
            pause_entree();
            break;
    }
}
// --- POINT D'ENTRÉE ---
// Charge les données, puis tourne en boucle infinie entre menu_connexion et menu_principal
int main(void) {
    Livre biblio[200];   // catalogue de livres, taille max fixée à 200
    int nb_livres = 0;
    // Chargement des utilisateurs depuis le fichier de données
    nb_users = charger_utilisateurs(users, MAX_UTILISATEURS);
    charger_livres(biblio, &nb_livres);
    if (nb_users == -1) {
        printf(ROUGE "Erreur : impossible de charger les utilisateurs.\n" RESET);
        return 1;
    }
    // Boucle principale : alterne entre connexion et menu selon l'état de la session
    while (1) {
        if (!connecte)
            menu_connexion();    // pas connecté → écran d'accueil
        else
            menu_principal(biblio, &nb_livres);   // connecté → menu des fonctionnalités
    }
    return 0;
}
