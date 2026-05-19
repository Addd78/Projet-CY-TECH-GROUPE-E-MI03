// utilisateurs.c — Lecture/ecriture fichier et gestion des comptes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilisateurs.h"
// Format d'une ligne dans utilisateurs.txt : login:motdepasse:role
// Exemple : Romuald:1234:0 prof_grignon:azerty:1
// role 0 = ETUDIANT, role 1 = PROFESSEUR
int charger_utilisateurs(Utilisateur tableau[], int taille_max) {
    // Ouvrir le fichier en lecture
    FILE *f = fopen(FICHIER_USERS, "r");
    if (f == NULL) {
        printf("  Erreur : impossible d'ouvrir %s\n", FICHIER_USERS);
        return -1;
    }
    int nb = 0;          // compteur d'utilisateurs lus
    int role_lu;         // variable temporaire pour lire le role (int)
    // Lire ligne par ligne tant qu'il y a des donnees et qu'on n'a pas dépassé la taille maximale du tableau
    while (nb < taille_max &&
           fscanf(f, "%63[^:]:%63[^:]:%d\n", tableau[nb].login, tableau[nb].mdp, &role_lu) == 3) {
        // Convertir l'entier lu en enum Role
        tableau[nb].role = (role_lu == 1) ? PROFESSEUR : ETUDIANT;
        nb++;
    }
    fclose(f);
    return nb;   // on retourne le nombre d'utilisateurs chargé
}
// Save les USERS
// Reecrit le fichier entierement avec le contenu du tableau.
// Appele apres chaque creation de compte.
int sauvegarder_utilisateurs(Utilisateur tableau[], int nb) {
    // Ouvrir en ecriture ("w" ecrase le fichier existant)
    FILE *f = fopen(FICHIER_USERS, "w");
    if (f == NULL) {
        printf("  Erreur : impossible d'ecrire dans %s\n", FICHIER_USERS);
        return -1;
    }
    // Ecrire chaque utilisateur sur une ligne
    for (int i = 0; i < nb; i++) {
        fprintf(f, "%s:%s:%d\n", tableau[i].login, tableau[i].mdp,(int)tableau[i].role);
    }
    fclose(f);
    return 0;
}
// Parcourt le tableau des utilisateurs et compare le login ET le mot de passe saisis.
// Si une correspondance est trouvee, copie l'utilisateur dans user_connecte et retourne 1.
// Retourne 0 si aucune correspondance.
int login(Utilisateur tableau[], int nb,
          const char *login_saisi, const char *mdp_saisi,
          Utilisateur *user_connecte) {
    for (int i = 0; i < nb; i++) {
        // strcmp retourne 0 si les deux chaines sont identiques
        int login_ok = (strcmp(tableau[i].login, login_saisi) == 0);
        int mdp_ok   = (strcmp(tableau[i].mdp,   mdp_saisi)   == 0);
        if (login_ok && mdp_ok) {
            // Copier l'utilisateur trouve dans la variable de sortie
            *user_connecte = tableau[i];
            return 1;   // succes
        }
    }
    return 0;   // echec : aucun utilisateur ne correspond
}
// Créer un compte
// Verifie d'abord que le login n'existe pas deja.
// Si libre, ajoute l'utilisateur dans le tableau ET dans le fichier.
// Retourne  1 si compte cree avec succes
// 0 si le login est deja pris
// -1 si erreur (tableau plein ou fichier inaccessible)
int creer_compte(Utilisateur tableau[], int *nb,
                 const char *login_saisi, const char *mdp_saisi,
                 Role role_choisi) {
    // Verifier que le tableau n'est pas plein
    if (*nb >= MAX_UTILISATEURS) {
        printf("  Erreur : nombre maximum d'utilisateurs atteint.\n");
        return -1;
    }
    // Verifier que le login n'existe pas deja
    for (int i = 0; i < *nb; i++) {
        if (strcmp(tableau[i].login, login_saisi) == 0) {
            printf("  Ce login est deja utilise.\n");
            return 0;   // login deja pris
        }
    }
    // Ajouter le nouvel utilisateur a la fin du tableau
    strncpy(tableau[*nb].login, login_saisi, TAILLE_LOGIN - 1);
    strncpy(tableau[*nb].mdp,   mdp_saisi,   TAILLE_MDP   - 1);
    tableau[*nb].role = role_choisi;
    // S'assurer que les chaines sont bien terminees par '\0'
    tableau[*nb].login[TAILLE_LOGIN - 1] = '\0';
    tableau[*nb].mdp[TAILLE_MDP   - 1] = '\0';
    (*nb)++;   // incrementer le compteur
    // Sauvegarder immediatement dans le fichier
    if (sauvegarder_utilisateurs(tableau, *nb) == -1) {
        return -1;
    }
    return 1;   // succes
}
