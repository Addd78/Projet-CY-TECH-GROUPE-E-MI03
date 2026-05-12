// Gestion des livres (Ajouter un livre)
// Algorythmes de Tri (par le titre ou l'auteur)
// Foction de recherche avec des mots-clé

// Dans le fichier livres.txt, les informations sont ecrites commme ID;Titre;Auteur;Categorie;Total;Dispo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/livres.h"

#define CLEAR "\033[2J\033[H"

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
    fclose(f);
    return 1;
}

void sauvegarder_livres(Livre biblio[], int nbLivres) {
    FILE *f = fopen("data/livres.txt", "w");
    if (f == NULL) return;
    for (int i = 0; i < nbLivres; i++){
        fprintf(f, "%d;%s;%s;%s;%d;%d\n",
            biblio[i].id,
            biblio[i].titre,
            biblio[i].auteur,
            biblio[i].quantite_totale,
            biblio[i].quantite_disponible);
    }
    fclose(f);
}

void afficher_les_livres(Livre biblio[], int nbLivres) {
    printf("\033[2J\033[H");
    printf("################ CATALOGUE DES LIVRES ################\n\n");

    if (nbLivres == 0) {
        printf(" [!] Aucun livre se trouve dans la base de donnees.\n");
    }
    else {
        printf("  ID   | TITRE                | AUTEUR          | DISPO\n");
        printf("  -----|----------------------|-----------------|------\n");

        for (int i = 0; i < nbLivres; i++) {
            printf("%-4d | %-20.20s | %-15.15s | %d/%d\n",
                biblio[i].id,
                biblio[i].titre,
                biblio[i].auteur,
                biblio[i].quantite_totale,
                biblio[i].quantite_disponible);
        }
    }

    printf("\n######################################################\n");
}

void rechercher_livre(Livre biblio[], int nbLivres) {
    char recherche[50];
    int trouve = 0;

    printf(CLEAR);
    printf("################ RECHERCHE DE LIVRES ################\n\n");
    printf("  Entrez un mot-cle (titre, auteur, categorie) : ");
    
    fgets(recherche, sizeof(recherche), stdin);

    scanf("%49[^\n]",recherche);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    recherche[strcspn(recherche, "\n")] = '\0';

    printf("\n  Resultats pour \"%s\" :\n", recherche);
    printf("  --------------------------------------------------\n");

    for (int i = 0; i < nbLivres; i++){
        if (strstr(biblio[i].titre, recherche) != NULL || 
            strstr(biblio[i].auteur, recherche) != NULL || 
            strstr(biblio[i].categorie, recherche) != NULL){
            
            printf("  [%d] %-20s | %-15s | %d dispo\n", 
                biblio[i].id,
                biblio[i].titre,
                biblio[i].auteur,
                biblio[i].quantite_totale,
                biblio[i].quantite_disponible);
            trouve = 1;
        }
        
    }
    
    if (!trouve) {
        printf("[!] Aucun livre ne correspond à votre recherche.\n");
    }

    printf("\n#####################################################\n");
}

// Ajout d'un livre (réservé aux professeurs dans le main)

void ajouter_livre(Livre biblio[], int *nbLivres) {
    printf(CLEAR);
    printf("################ AJOUTER UN LIVRE ################\n\n");
    
    Livre n;
    n.id = (*nbLivres > 0) ? biblio[*nbLivres - 1].id + 1 : 1;

    printf("  Titre : ");
    fgets(n.titre, 100, stdin); n.titre[strcspn(n.titre, "\n")] = '\0';
    printf("  Auteur : ");
    fgets(n.auteur, 100, stdin); n.auteur[strcspn(n.auteur, "\n")] = '\0';
    printf("  Categorie : ");
    fgets(n.categorie, 50, stdin); n.categorie[strcspn(n.categorie, "\n")] = '\0';
    printf("  Quantite totale : ");
    scanf("%d", &n.quantite_totale);
    n.quantite_disponible = n.quantite_totale;
    getchar();

    biblio[*nbLivres] = n;
    (*nbLivres)++;
    sauvegarder_livres(biblio, *nbLivres);
    printf("\n  [V] Livre ajoute avec succes !\n");
}
