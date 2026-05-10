// verifier s'il reste des exemplaires en stock et si l'utilisateur peut emprunter (si etudiant ou prof)
#include <stdio.h>
#include <string.h>


#include "../include/utilisateurs.h"
#include "../include/livres.h" 
#include "../include/emprunts.h"

#define FICHIER_EMPRUNTS "data/emprunts.txt"

static int livre_max;
static int temps_max;            // voir si possible de faire tourner avec les 2 max ds la struct livre 
 
void appliquer_regles_role(Utilisateur *personne) {
    if (personne->role == PROFESSEUR) {
        livre_max = 5;
        temps_max = 180;
    } else {
        livre_max = 3;
        temps_max = 120;
    }
}
 
int verif_emprunt(Utilisateur *personne, int temps_actuel, int livres_empruntes, int nb_livres_retard, Livre *l) {

    appliquer_regles_role(personne);

// Verification des livres disponible
    if (l->quantite_disponible <= 0) {    //a le droit d'emprunter mais livre en rupture
        printf("Refus : plus aucun exemplaire disponible.\n");
        return 0;
    }
 
// Verification des retards
    if (nb_livres_retard > 0) {
        printf("Refus : vous avez %d livre(s) en retard, rendez-les d'abord !\n", nb_livres_retard);
        return 0;
    }

// verification du nombre de livre emprunté
    if (livres_empruntes >= livre_max) {
        printf("Refus : limite de livres depassee (%d/%d).\n", livres_empruntes, livre_max);
        return 0;
    }

// Verification de la duree
    if (temps_actuel > temps_max) {
        printf("Refus : temps depasse (%ds/%ds).\n", temps_actuel, temps_max);
        return 0;
    }
 
    printf("Emprunt autorise ! (%d/%d livres)\n", livres_empruntes + 1, livre_max);
    return 1;
}

void emprunter_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;

    printf("\n--- EMPRUNTER UN LIVRE ---\n");
    printf("  Entrez l'ID du livre que vous souhaitez emprunter : ");
    if (scanf("%d", &id_voulu) != 1) {
        printf("  Erreur de saisie.\n");
        while (getchar() != '\n'); // Purge
        return;
    }
    while (getchar() != '\n'); // Purge le buffer après scanf

    // Rechercher le livre dans le tableau par son ID
    for (int i = 0; i < nb_livres; i++) {
        if (biblio[i].id == id_voulu) {
            index_livre = i;
            break;
        }
    }

    if (index_livre == -1) {
        printf("  [!] Livre avec l'ID %d introuvable.\n", id_voulu);
        return;
    }

    // Vérifier les conditions (on simule ici 0 retard et 0 livre déjà emprunté)
    if (verif_emprunt(user, 0, 0, 0, &biblio[index_livre])) {
        
        // Mettre à jour le stock en mémoire
        biblio[index_livre].quantite_disponible--;

        // Sauvegarder les modifications dans le fichier livres.txt
        sauvegarder_livres(biblio, nb_livres);

        printf("  [V] Succes ! Vous avez recupere : %s\n", biblio[index_livre].titre);
        printf("      Il reste %d exemplaire(s) en rayon.\n", biblio[index_livre].quantite_disponible);
    }
}

void rendre_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;

    printf("\n--- RENDRE UN LIVRE ---\n");
    printf("  Entrez l'ID du livre que vous souhaitez rendre : ");
    
    // Lecture de l'ID avec sécurisation du buffer
    if (scanf("%d", &id_voulu) != 1) {
        printf("  Erreur de saisie.\n");
        while (getchar() != '\n'); 
        return;
    }
    while (getchar() != '\n'); 

    // 1. Rechercher le livre dans le catalogue
    for (int i = 0; i < nb_livres; i++) {
        if (biblio[i].id == id_voulu) {
            index_livre = i;
            break;
        }
    }

    if (index_livre == -1) {
        printf("  [!] Erreur : Le livre ID %d n'appartient pas au catalogue.\n", id_voulu);
        return;
    }

    // 2. Vérification de cohérence (ne pas dépasser le stock total)
    if (biblio[index_livre].quantite_disponible >= biblio[index_livre].quantite_totale) {
        printf("  [!] Erreur : Tous les exemplaires de '%s' sont deja en rayon.\n", biblio[index_livre].titre);
        return;
    }

    // 3. Mise à jour du stock en mémoire
    biblio[index_livre].quantite_disponible++;

    // 4. Sauvegarde persistante dans le fichier data/livres.txt
    sauvegarder_livres(biblio, nb_livres);

    printf("  [V] Succes ! Vous avez rendu : %s\n", biblio[index_livre].titre);
    printf("      Nouveau stock disponible : %d/%d\n", 
           biblio[index_livre].quantite_disponible, 
           biblio[index_livre].quantite_totale);
}

void afficher_emprunts_utilisateur(Livre biblio[], int nb_livres, Utilisateur *user) {
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) {
        printf("\n  [!] Aucun historique d'emprunt trouve.\n");
        return;
    }

    Emprunt e;
    int trouve = 0;
    printf("\n--- VOS EMPRUNTS EN COURS ---\n");
    printf("  ID   | TITRE DU LIVRE\n");
    printf("  -----|----------------------\n");

    // Lecture du fichier : login:id_livre:rendu
    while (fscanf(f, "%[^:]:%d:%d\n", e.login, &e.id_livre, &e.rendu) != EOF) {
        // On vérifie si l'emprunt appartient à l'utilisateur et n'est pas rendu
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            
            // Chercher le titre dans le tableau biblio
            for (int i = 0; i < nb_livres; i++) {
                if (biblio[i].id == e.id_livre) {
                    printf("  %-4d | %s\n", biblio[i].id, biblio[i].titre);
                    trouve = 1;
                    break;
                }
            }
        }
    }

    if (!trouve) {
        printf("  Vous n'avez aucun emprunt en cours.\n");
    }
    
    printf("  ----------------------------\n");
    fclose(f);
}