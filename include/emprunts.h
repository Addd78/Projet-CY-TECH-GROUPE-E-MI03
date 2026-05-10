#ifndef EMPRUNTS_H
#define EMPRUNTS_H

#include "livres.h"
#include "utilisateurs.h"

typedef struct {
    int id_livre;
    char login[TAILLE_LOGIN];
    char date_emprunt[11]; // format JJ/MM/AAAA
    int rendu; // 0 pour en cours, 1 pour rendu
} Emprunt;

void afficher_emprunts_utilisateur(Livre biblio[], int nb_Livres, Utilisateur *user);

void appliquer_regles_role(Utilisateur *personne);

int verif_emprunt(Utilisateur *personne, int temps_actuel, int livres_empruntes, int nb_livres_retard, Livre *l);

#endif