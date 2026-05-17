#ifndef LIVRES_H
#define LIVRES_H
#define MAX_STR 100

// Structure du livre : ID;Titre;Auteur;Categorie;Dispo;Totale
typedef struct {
    int id;
    char titre[MAX_STR];
    char auteur[MAX_STR];
    char categorie[MAX_STR];
    int quantite_disponible;
    int quantite_totale;
} Livre;

// Charge les livres depuis le fichier data/livres.txt.
int charger_livres(Livre biblio[], int *nbLivres);

// Sauvegarde le tableau de livres actuel dans data/livres.txt.
void sauvegarder_livres(Livre biblio[], int nbLivres);

// Affiche la liste complète des livres
void afficher_les_livres(Livre biblio[], int nbLivres);

// Recherche un livre par mot-clé
void rechercher_livre(Livre biblio[], int nbLivres);

// ajout d'un livre au catalogue
void ajouter_livre(Livre biblio[], int *nbLivres);

#endif