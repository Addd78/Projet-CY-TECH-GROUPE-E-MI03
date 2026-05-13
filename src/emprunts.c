// emprunts.c — Gestion des emprunts et rendus de livres

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../include/utilisateurs.h"
#include "../include/livres.h"
#include "../include/emprunts.h"

#define FICHIER_EMPRUNTS "data/emprunts.txt"

static int livre_max;
static int temps_max;

// ─────────────────────────────────────────────
//  REGLES SELON LE ROLE
// ─────────────────────────────────────────────
void appliquer_regles_role(Utilisateur *personne) {
    if (personne->role == PROFESSEUR) {
        livre_max = 5;
        temps_max = 180;
    } else {
        livre_max = 3;
        temps_max = 120;
    }
}

// ─────────────────────────────────────────────
//  VERIFICATION AVANT EMPRUNT
// ─────────────────────────────────────────────
int verif_emprunt(Utilisateur *personne, int temps_actuel,
                  int livres_empruntes, int nb_livres_retard, Livre *l) {

    appliquer_regles_role(personne);

    // Livre disponible en stock ?
    if (l->quantite_disponible <= 0) {
        printf("Refus : plus aucun exemplaire disponible.\n");
        return 0;
    }

    // Livres en retard non rendus ?
    if (nb_livres_retard > 0) {
        printf("Refus : vous avez %d livre(s) en retard, rendez-les d'abord !\n",
               nb_livres_retard);
        return 0;
    }

    // Quota de livres atteint ?
    if (livres_empruntes >= livre_max) {
        printf("Refus : limite de livres depassee (%d/%d).\n",
               livres_empruntes, livre_max);
        return 0;
    }
    /*
    if (livres_empruntes >= ) {
        printf("Refus : limite de livres depassee (%d/%d).\n",
               livres_empruntes, livre_max);
        return 0;
    }
    */
    
    // Temps depasse ?
    if (temps_actuel > temps_max) {
        printf("Refus : temps depasse (%ds/%ds).\n", temps_actuel, temps_max);
        return 0;
    }

    printf("Emprunt autorise ! (%d/%d livres)\n", livres_empruntes + 1, livre_max);
    return 1;
}


// ─────────────────────────────────────────────
//  COMPTER LES EMPRUNTS ACTIFS
// ─────────────────────────────────────────────
int compter_livres_empruntes(Utilisateur *user) {
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) return 0; // Si le fichier n'existe pas, 0 emprunts

    Emprunt e;
    int count = 0;
    // On lit le fichier et on compte les livres non rendus par cet utilisateur
    while (fscanf(f, "%[^:]:%d:%[^:]:%d\n", e.login, &e.id_livre, e.date_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            count++;
        }
    }
    fclose(f);
    return count;
}


// ─────────────────────────────────────────────
//  EMPRUNTER UN LIVRE
// ─────────────────────────────────────────────
void emprunter_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;

    printf("\n--- EMPRUNTER UN LIVRE ---\n");
    printf("  Entrez l'ID du livre que vous souhaitez emprunter : ");
    if (scanf("%d", &id_voulu) != 1) {
        printf("  Erreur de saisie.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Rechercher le livre par ID
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

    // Compter combien de livres l'utilisateur a déjà
    int emprunts_actuels = compter_livres_empruntes(user);

    // Verifier les conditions d'emprunt en passant le vrai nombre
    if (verif_emprunt(user, 0, emprunts_actuels, 0, &biblio[index_livre])) {

        // Mettre a jour le stock en memoire
        biblio[index_livre].quantite_disponible--;

        // Sauvegarder dans livres.txt
        sauvegarder_livres(biblio, nb_livres);

        // Enregistrer l'emprunt dans emprunts.txt
        FILE *fe = fopen(FICHIER_EMPRUNTS, "a");
        if (fe != NULL) {
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            char date_str[11];
            strftime(date_str, sizeof(date_str), "%d/%m/%Y", tm_info);
            fprintf(fe, "%s:%d:%s:0\n", user->login, biblio[index_livre].id, date_str);
            fclose(fe);
        }

        printf("  [V] Succes ! Vous avez recupere : %s\n",
               biblio[index_livre].titre);
        printf("      Il reste %d exemplaire(s) en rayon.\n",
               biblio[index_livre].quantite_disponible);
    }
}

// ─────────────────────────────────────────────
//  AFFICHER LES EMPRUNTS EN COURS
// ─────────────────────────────────────────────
// Version unique conservee : affiche ID, date d'emprunt et titre du livre
void afficher_emprunts_utilisateur(Livre biblio[], int nb_livres, Utilisateur *user) {
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) {
        printf("\n  [!] Aucun historique d'emprunt trouve.\n");
        return;
    }

    Emprunt e;
    int trouve = 0;
    printf("\n--- VOS EMPRUNTS EN COURS ---\n");
    printf("  ID   | DATE EMPRUNT | TITRE DU LIVRE\n");
    printf("  -----|--------------|----------------------\n");

    // Format du fichier : login:id_livre:date:rendu
    while (fscanf(f, "%[^:]:%d:%[^:]:%d\n",
                  e.login, &e.id_livre, e.date_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            for (int i = 0; i < nb_livres; i++) {
                if (biblio[i].id == e.id_livre) {
                    printf("  %-4d | %-12s | %s\n",
                           biblio[i].id, e.date_emprunt, biblio[i].titre);
                    trouve = 1;
                    break;
                }
            }
        }
    }

    if (!trouve) {
        printf("  Vous n'avez aucun emprunt en cours.\n");
    }

    printf("  -------------------------------------------\n");
    fclose(f);
}

// ─────────────────────────────────────────────
//  RENDRE UN LIVRE
// ─────────────────────────────────────────────
void rendre_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;

    printf("\n--- RENDRE UN LIVRE ---\n");
    printf("  Entrez l'ID du livre que vous souhaitez rendre : ");
    if (scanf("%d", &id_voulu) != 1) {
        printf("  Erreur de saisie.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Rechercher le livre dans le catalogue
    for (int i = 0; i < nb_livres; i++) {
        if (biblio[i].id == id_voulu) {
            index_livre = i;
            break;
        }
    }

    if (index_livre == -1) {
        printf("  [!] Erreur : Le livre ID %d n'appartient pas au catalogue.\n",
               id_voulu);
        return;
    }

    // Verifier qu'on ne depasse pas le stock total
    if (biblio[index_livre].quantite_disponible >= biblio[index_livre].quantite_totale) {
        printf("  [!] Erreur : Tous les exemplaires de '%s' sont deja en rayon.\n",
               biblio[index_livre].titre);
        return;
    }

    // Marquer l'emprunt comme rendu dans emprunts.txt
    FILE *f_in = fopen(FICHIER_EMPRUNTS, "r");
    if (f_in != NULL) {
        char tmp_path[] = "data/emprunts_tmp.txt";
        FILE *f_out = fopen(tmp_path, "w");
        if (f_out != NULL) {
            Emprunt e;
            int updated = 0;
            while (fscanf(f_in, "%[^:]:%d:%[^:]:%d\n",
                          e.login, &e.id_livre, e.date_emprunt, &e.rendu) == 4) {
                if (!updated && strcmp(e.login, user->login) == 0
                    && e.id_livre == id_voulu && e.rendu == 0) {
                    e.rendu = 1;
                    updated = 1;
                }
                fprintf(f_out, "%s:%d:%s:%d\n",
                        e.login, e.id_livre, e.date_emprunt, e.rendu);
            }
            fclose(f_out);
            if (!updated) {
                printf("  [!] Avertissement : aucun emprunt actif trouve pour ce livre.\n");
            }
            remove(FICHIER_EMPRUNTS);
            rename(tmp_path, FICHIER_EMPRUNTS);
        }
        fclose(f_in);
    }

    // Mettre a jour le stock
    biblio[index_livre].quantite_disponible++;

    // Sauvegarder dans livres.txt
    sauvegarder_livres(biblio, nb_livres);

    printf("  [V] Succes ! Vous avez rendu : %s\n", biblio[index_livre].titre);
    printf("      Nouveau stock disponible : %d/%d\n",
           biblio[index_livre].quantite_disponible,
           biblio[index_livre].quantite_totale);
}

// ─────────────────────────────────────────────
//  AFFICHER LES RETARDS
// ─────────────────────────────────────────────
void afficher_retards(Livre biblio[], int nb_livres, Utilisateur *user) {
    appliquer_regles_role(user);

    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) {
        printf("\n  [!] Impossible d'acceder aux donnees d'emprunts.\n");
        return;
    }

    Emprunt e;
    int retards_trouves = 0;

    printf("\n--- VOS RETARDS ---\n");

    // Format du fichier : login:id_livre:date:rendu
    while (fscanf(f, "%[^:]:%d:%[^:]:%d\n",
                  e.login, &e.id_livre, e.date_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            for (int i = 0; i < nb_livres; i++) {
                if (biblio[i].id == e.id_livre) {
                    printf("  [!] RETARD POTENTIEL : %s (Emprunte le %s)\n",
                           biblio[i].titre, e.date_emprunt);
                    printf("      Rappel : Votre limite est de %d secondes.\n",
                           temps_max);
                    retards_trouves = 1;
                }
            }
        }
    }

    if (!retards_trouves) {
        printf("  Felicitations, vous n'avez aucun retard enregistre.\n");
    }

    fclose(f);
}
