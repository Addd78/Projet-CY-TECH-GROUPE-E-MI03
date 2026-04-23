// Gestion des livres (Ajouter ou supprimer un livre)
// Algorythmes de Tri (par le titre ou l'auteur)
// Foction de recherche avec des mots-clé

// Dans le fichier livres.txt, les informations sont ecrites commme ID;Titre;Auteur;Categorie;Total;Dispo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "livres.h"

int charger_livres(Livre biblio[], int *nbLivres){
    FILE *f = fopen("data/livres.txt", "r");
    if (f == NULL){
        printf("Erreur : Impossible d'ouvrir le fichier data/livres.txt\n");
        return 0;
    }

    *nbLivres = 0;

    while (fscanf(f, "%d; %[^;]; %[^;]; %[^;]; %d; %d",
        &biblio[*nbLivres].id, 
        biblio[*nbLivres].titre, 
        biblio[*nbLivres].auteur,
        biblio[*nbLivres].categorie,
        &biblio[*nbLivres].quantite_totale,
        &biblio[*nbLivres].quantite_disponible) != EOF) {
            (*nbLivres)++;
    }
}

void afficher_les_livres(Livre biblio[], int nbLivres) {
    //Je sais pas comment on va afficher :)

    for (int i = 0; i < nbLivres; i++) {
        printf("%d | %s | %s | %d/%d",
            biblio[i].id,
            biblio[i].titre,
            biblio[i].auteur,
            biblio[i].quantite_disponible,
            biblio[i].quantite_totale);
    }
}