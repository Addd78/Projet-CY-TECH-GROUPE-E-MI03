#ifndef EMPRUNTS_H
#define EMPRUNTS_H

#include "livres.h"
#include "utilisateurs.h"

// ─────────────────────────────────────────────
//  STRUCTURE EMPRUNT
// ─────────────────────────────────────────────
// Le champ timestamp_emprunt stocke le nombre de secondes
// depuis le 01/01/1970 (time_t) au moment de l'emprunt.
// Cela permet de calculer un delai en secondes avec precision.
// Format dans emprunts.txt : login:id_livre:timestamp:rendu
typedef struct {
    char  login[64];
    int   id_livre;
    long  timestamp_emprunt;   // remplace date_emprunt en string
    int   rendu;               // 0 = en cours, 1 = rendu
} Emprunt;

// Applique les regles (livre_max, temps_max) selon le role de l'utilisateur
void appliquer_regles_role(Utilisateur *personne);

// Compte les livres actuellement empruntes et non rendus par l'utilisateur
int compter_livres_empruntes(Utilisateur *user);

// Compte les livres en retard (delai depasse) de l'utilisateur
int compter_retards(Utilisateur *user);

// Verifie si un utilisateur a le droit d'emprunter un livre specifique
int verif_emprunt(Utilisateur *personne, int livres_empruntes,
                  int nb_livres_retard, Livre *l);

// Gere la procedure d'emprunt
void emprunter_livre(Livre biblio[], int nb_livres, Utilisateur *user);

// Gere le retour d'un livre
void rendre_livre(Livre biblio[], int nb_livres, Utilisateur *user);

// Affiche les livres actuellement detenus par l'utilisateur
void afficher_emprunts_utilisateur(Livre biblio[], int nb_livres, Utilisateur *user);

// Affiche les livres dont le delai de restitution est depasse
void afficher_retards(Livre biblio[], int nb_livres, Utilisateur *user);

#endif
