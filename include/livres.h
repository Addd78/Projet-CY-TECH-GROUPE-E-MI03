#ifndef LIVRES_H
#define LIVRES_H
#define MAX_STR 100

typedef struct {
    int id;
    char titre[MAX_STR];
    char auteur[MAX_STR];
    char categorie[MAX_STR];
    int quantite_totale;
    int quantite_disponible;
} Livre;

int chargerLivres(Livre biblio[], int *nbLivres);

// appelle des fonctions dans src/livres.c

#endif