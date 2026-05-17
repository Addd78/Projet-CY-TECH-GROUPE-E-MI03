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

void appliquer_regles_role(Utilisateur *personne);

// Compte les livres actuellement empruntes et non rendus par l'utilisateur
int compter_livres_empruntes(Utilisateur *user);

// Compte les livres en retard (delai depasse) de l'utilisateur
int compter_retards(Utilisateur *user);

// Verifie si un utilisateur a le droit d'emprunter un livre specifique
int verif_emprunt(Utilisateur *personne, int temps_actuel, int livres_empruntes, int nb_livres_retard, Livre *l);

// Gère la procédure d'emprunt (Saisie ID -> Vérification -> Mise à jour stock)
void emprunter_livre(Livre biblio[], int nb_livres, Utilisateur *user);

// Gère le retour d'un livre (Saisie ID -> Mise à jour stock)
void rendre_livre(Livre biblio[], int nb_livres, Utilisateur *user);

// Affiche la liste des livres actuellement détenus par l'utilisateur (Option 3 du menu)
void afficher_emprunts_utilisateur(Livre biblio[], int nb_livres, Utilisateur *user);

// Affiche les livres dont le délai de restitution est dépassé (Option 7 du menu)
void afficher_retards(Livre biblio[], int nb_livres, Utilisateur *user);

#endif