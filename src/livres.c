// Gestion des livres (Ajouter un livre)
// Algorythmes de Tri (par le titre ou l'auteur)
// Fonction de recherche avec des mots-clé
// Dans le fichier livres.txt on a mis les info comme tel :  ID;Titre;Auteur;Categorie;Total;Dispo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/livres.h"
// Séquence ANSI pour pouvoir clear et réafficher le terminal (trouvé sur internet) : (\033 : Echap , [2J : Efface tout l'écran, \033[H : Remet le curseur en haut a gauche) 
#define CLEAR "\033[2J\033[H"
/*
 * Ouvre le fichier livres.txt et charge son contenu dans le tableau biblio.
 * Chaque ligne du fichier correspond a un livre avec ses informations.
 * Retourne 1 si le chargement s'est bien passe, 0 en cas d'erreur.
 */
int charger_livres(Livre biblio[], int *nbLivres){
    FILE *f = fopen("data/livres.txt", "r");
    if (f == NULL){
        printf("Erreur : Impossible d'ouvrir le fichier data/livres.txt\n");
        return 0;
    }
    *nbLivres = 0;
    while (fscanf(f, "%d;%[^;];%[^;];%[^;];%d;%d", &biblio[*nbLivres].id, biblio[*nbLivres].titre, biblio[*nbLivres].auteur, biblio[*nbLivres].categorie, &biblio[*nbLivres].quantite_disponible, &biblio[*nbLivres].quantite_totale) == 6) {
            (*nbLivres)++;
    }
    fclose(f);
    return 1;
}
/*
 * Ecrit le contenu du tableau biblio dans le fichier livres.txt.
 * Le fichier est entierement reecrit a chaque appel.
 * Appelee apres chaque emprunt, rendu ou ajout de livre.
 */
void sauvegarder_livres(Livre biblio[], int nbLivres) {
    FILE *f = fopen("data/livres.txt", "w");
    if (f == NULL) return;
    for (int i = 0; i < nbLivres; i++){
        fprintf(f, "%d;%s;%s;%s;%d;%d\n", biblio[i].id, biblio[i].titre, biblio[i].auteur, biblio[i].categorie, biblio[i].quantite_disponible, biblio[i].quantite_totale);
    }
    fclose(f);
}
/*
 * Affiche la liste complete de tous les livres disponibles dans la bibliotheque.
 * Pour chaque livre, on affiche son ID, son titre, son auteur
 * et le nombre d'exemplaires disponibles sur le total.
 */
void afficher_les_livres(Livre biblio[], int nbLivres) {
    printf("\033[2J\033[H");
    printf("################ CATALOGUE DES LIVRES ################\n\n");
    if (nbLivres == 0) {
        printf("[!] Aucun livre se trouve dans la base de donnees.\n");
    }
    else {
        printf("ID   | TITRE                | AUTEUR          | DISPO\n");
        printf("-----|----------------------|-----------------|------\n");
        for (int i = 0; i < nbLivres; i++) {
            printf("%-4d | %-20.20s | %-15.15s | %d/%d\n", biblio[i].id, biblio[i].titre, biblio[i].auteur, biblio[i].quantite_disponible, biblio[i].quantite_totale);
        }
    }
    printf("\n######################################################\n");
}
/*
 * Demande a l'utilisateur un mot-cle et cherche ce mot dans les titres,
 * les auteurs et les categories de tous les livres du catalogue.
 * Affiche tous les livres qui contiennent ce mot-cle.
 */
void rechercher_livre(Livre biblio[], int nbLivres) {
    char recherche[50];
    int trouve = 0;
    printf(CLEAR);
    printf("################ RECHERCHE DE LIVRES ################\n\n");
    printf("Entrez un mot-cle (titre, auteur, categorie) : ");
    fgets(recherche, sizeof(recherche), stdin);
    recherche[strcspn(recherche, "\n")] = '\0';
    printf("\nResultats pour \"%s\" :\n", recherche);
    printf("--------------------------------------------------\n");
    for (int i = 0; i < nbLivres; i++){
        if (strstr(biblio[i].titre, recherche) != NULL || strstr(biblio[i].auteur, recherche) != NULL || strstr(biblio[i].categorie, recherche) != NULL){
            printf("[%d] %-20s | %-15s | %d/%d dispo\n", biblio[i].id, biblio[i].titre, biblio[i].auteur, biblio[i].quantite_disponible, biblio[i].quantite_totale);
            trouve = 1;
        } 
    }
    if (!trouve) {
        printf("[!] Aucun livre ne correspond à votre recherche.\n");
    }
    printf("\n#####################################################\n");
}
/*
 * Permet a un professeur d'ajouter un nouveau livre dans le catalogue.
 * Demande le titre, l'auteur, la categorie et la quantite disponible.
 * Le nouveau livre est ensuite sauvegarde dans le fichier livres.txt.
 * Cette fonction est reservee aux professeurs, la verification du role
 * est faite dans le menu principal avant d'appeler cette fonction.
 */
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
    printf("\n[V] Livre ajoute avec succes !\n");
}
