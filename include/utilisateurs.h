// utilisateurs.h — Declarations des fonctions et structures liees aux utilisateurs

#ifndef UTILISATEURS_H
#define UTILISATEURS_H

// ─────────────────────────────────────────────
//  CONSTANTES
// ─────────────────────────────────────────────
#define MAX_UTILISATEURS 100
#define TAILLE_LOGIN     64
#define TAILLE_MDP       64
#define FICHIER_USERS    "data/utilisateurs.txt"

// ─────────────────────────────────────────────
//  ROLES
// ─────────────────────────────────────────────
typedef enum {
    ETUDIANT   = 0,
    PROFESSEUR = 1
} Role;

// ─────────────────────────────────────────────
//  STRUCTURE UTILISATEUR
// ─────────────────────────────────────────────
typedef struct {
    char login[TAILLE_LOGIN];
    char mdp[TAILLE_MDP];
    Role role;
} Utilisateur;

// ─────────────────────────────────────────────
//  DECLARATIONS DES FONCTIONS
// ─────────────────────────────────────────────

// Charge tous les utilisateurs depuis le fichier dans le tableau
// Retourne le nombre d'utilisateurs charges, -1 si erreur
int charger_utilisateurs(Utilisateur tableau[], int taille_max);

// Sauvegarde tous les utilisateurs du tableau dans le fichier
// Retourne 0 si succes, -1 si erreur
int sauvegarder_utilisateurs(Utilisateur tableau[], int nb);

// Tente de connecter un utilisateur
// Remplit user_connecte si succes
// Retourne 1 si login reussi, 0 sinon
int login(Utilisateur tableau[], int nb,
          const char *login_saisi, const char *mdp_saisi,
          Utilisateur *user_connecte);

// Cree un nouveau compte et l'ajoute dans le fichier
// Retourne 1 si succes, 0 si le login existe deja, -1 si erreur
int creer_compte(Utilisateur tableau[], int *nb,
                 const char *login_saisi, const char *mdp_saisi,
                 Role role_choisi);

#endif
