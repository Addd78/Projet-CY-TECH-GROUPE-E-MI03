// emprunts.c — Gestion des emprunts et rendus de livres

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../include/utilisateurs.h"
#include "../include/livres.h"
#include "../include/emprunts.h"

#define FICHIER_EMPRUNTS "data/emprunts.txt"

// ─────────────────────────────────────────────
//  LIMITES SELON LE ROLE
//  Etudiant  : 3 livres max, 120 secondes (2 min)
//  Professeur: 5 livres max, 180 secondes (3 min)
// ─────────────────────────────────────────────
static int livre_max;
static int temps_max;

void appliquer_regles_role(Utilisateur *personne) {
    if (personne->role == PROFESSEUR) {
        livre_max = 5;
        temps_max = 180;   // 3 minutes en secondes
    } else {
        livre_max = 3;
        temps_max = 120;   // 2 minutes en secondes
    }
}

// ─────────────────────────────────────────────
//  COMPTER LES EMPRUNTS ACTIFS
// ─────────────────────────────────────────────
// Lit emprunts.txt et compte les lignes non rendues de cet utilisateur.
int compter_livres_empruntes(Utilisateur *user) {
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) return 0;

    Emprunt e;
    int count = 0;
    // Format : login:id_livre:timestamp:rendu
    while (fscanf(f, "%63[^:]:%d:%ld:%d\n",
                  e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            count++;
        }
    }
    fclose(f);
    return count;
}

// ─────────────────────────────────────────────
//  COMPTER LES LIVRES EN RETARD
// ─────────────────────────────────────────────
// Un livre est en retard si (maintenant - timestamp_emprunt) > temps_max.
// On compare en secondes, ce qui permet une precision a la seconde pres.
int compter_retards(Utilisateur *user) {
    appliquer_regles_role(user);

    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) return 0;

    Emprunt e;
    int count = 0;
    time_t maintenant = time(NULL);   // timestamp actuel

    while (fscanf(f, "%63[^:]:%d:%ld:%d\n",
                  e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            // Calcul du temps ecoule depuis l'emprunt
            long secondes_ecoules = (long)difftime(maintenant,
                                    (time_t)e.timestamp_emprunt);
            if (secondes_ecoules > temps_max) {
                count++;
            }
        }
    }
    fclose(f);
    return count;
}

// ─────────────────────────────────────────────
//  VERIFICATION AVANT EMPRUNT
// ─────────────────────────────────────────────
// Retourne 1 si l'emprunt est autorise, 0 sinon.
// Les vrais compteurs (emprunts actifs, retards) sont calcules
// dans emprunter_livre() et passes ici.
int verif_emprunt(Utilisateur *personne, int livres_empruntes,
                  int nb_livres_retard, Livre *l) {

    appliquer_regles_role(personne);

    // 1. Livre disponible en stock ?
    if (l->quantite_disponible <= 0) {
        printf("  Refus : plus aucun exemplaire disponible.\n");
        return 0;
    }

    // 2. L'utilisateur a-t-il des livres en retard ?
    //    Si oui, il doit les rendre TOUS avant de pouvoir emprunter.
    if (nb_livres_retard > 0) {
        printf("  Refus : vous avez %d livre(s) en retard.\n", nb_livres_retard);
        printf("  Rendez tous vos livres en retard avant d'emprunter a nouveau.\n");
        return 0;
    }

    // 3. Quota de livres simultanement atteint ?
    if (livres_empruntes >= livre_max) {
        printf("  Refus : vous avez atteint la limite de %d livre(s).\n", livre_max);
        return 0;
    }

    printf("  Emprunt autorise ! (%d/%d livres)\n", livres_empruntes + 1, livre_max);
    return 1;
}

// ─────────────────────────────────────────────
//  EMPRUNTER UN LIVRE
// ─────────────────────────────────────────────
void emprunter_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;

    printf("\n--- EMPRUNTER UN LIVRE ---\n");
    printf("  Entrez l'ID du livre : ");
    if (scanf("%d", &id_voulu) != 1) {
        printf("  Erreur de saisie.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    // Rechercher le livre par ID dans le catalogue
    for (int i = 0; i < nb_livres; i++) {
        if (biblio[i].id == id_voulu) {
            index_livre = i;
            break;
        }
    }
    if (index_livre == -1) {
        printf("  [!] Livre ID %d introuvable.\n", id_voulu);
        return;
    }

    // Calculer les compteurs reels depuis emprunts.txt
    int emprunts_actuels = compter_livres_empruntes(user);
    int retards_actuels  = compter_retards(user);

    // Verifier les conditions
    if (verif_emprunt(user, emprunts_actuels, retards_actuels,
                      &biblio[index_livre])) {

        // Decremente le stock en memoire
        biblio[index_livre].quantite_disponible--;
        sauvegarder_livres(biblio, nb_livres);

        // Enregistrer l'emprunt avec le timestamp actuel (en secondes)
        FILE *fe = fopen(FICHIER_EMPRUNTS, "a");
        if (fe != NULL) {
            long ts = (long)time(NULL);   // nombre de secondes depuis 01/01/1970
            fprintf(fe, "%s:%d:%ld:0\n", user->login, biblio[index_livre].id, ts);
            fclose(fe);
        }

        printf("  [V] Succes ! Vous avez emprunte : %s\n",
               biblio[index_livre].titre);
        printf("      Il reste %d exemplaire(s) en rayon.\n",
               biblio[index_livre].quantite_disponible);

        // Rappeler la limite de temps
        appliquer_regles_role(user);
        printf("      Vous devez le rendre dans %d seconde(s).\n", temps_max);
    }
}

// ─────────────────────────────────────────────
//  RENDRE UN LIVRE
// ─────────────────────────────────────────────
void rendre_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;

    printf("\n--- RENDRE UN LIVRE ---\n");
    printf("  Entrez l'ID du livre a rendre : ");
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
        printf("  [!] Livre ID %d inconnu du catalogue.\n", id_voulu);
        return;
    }

    // Verifier que ce livre n'est pas deja entierement en rayon
    if (biblio[index_livre].quantite_disponible >=
        biblio[index_livre].quantite_totale) {
        printf("  [!] Tous les exemplaires de '%s' sont deja en rayon.\n",
               biblio[index_livre].titre);
        return;
    }

    // Marquer l'emprunt comme rendu dans emprunts.txt
    // Technique : lire dans un fichier temporaire, réécrire avec rendu=1
    FILE *f_in = fopen(FICHIER_EMPRUNTS, "r");
    if (f_in != NULL) {
        FILE *f_out = fopen("data/emprunts_tmp.txt", "w");
        if (f_out != NULL) {
            Emprunt e;
            int updated = 0;
            while (fscanf(f_in, "%63[^:]:%d:%ld:%d\n",
                          e.login, &e.id_livre,
                          &e.timestamp_emprunt, &e.rendu) == 4) {
                // On marque uniquement le premier emprunt actif correspondant
                if (!updated
                    && strcmp(e.login, user->login) == 0
                    && e.id_livre == id_voulu
                    && e.rendu == 0) {
                    e.rendu = 1;
                    updated = 1;
                }
                fprintf(f_out, "%s:%d:%ld:%d\n",
                        e.login, e.id_livre, e.timestamp_emprunt, e.rendu);
            }
            fclose(f_out);
            if (!updated) {
                printf("  [!] Aucun emprunt actif trouve pour ce livre.\n");
                remove("data/emprunts_tmp.txt");
            } else {
                remove(FICHIER_EMPRUNTS);
                rename("data/emprunts_tmp.txt", FICHIER_EMPRUNTS);
            }
        }
        fclose(f_in);
    }

    // Mettre a jour le stock
    biblio[index_livre].quantite_disponible++;
    sauvegarder_livres(biblio, nb_livres);

    printf("  [V] Succes ! Vous avez rendu : %s\n", biblio[index_livre].titre);
    printf("      Stock disponible : %d/%d\n",
           biblio[index_livre].quantite_disponible,
           biblio[index_livre].quantite_totale);
}

// ─────────────────────────────────────────────
//  AFFICHER LES EMPRUNTS EN COURS
// ─────────────────────────────────────────────
void afficher_emprunts_utilisateur(Livre biblio[], int nb_livres,
                                   Utilisateur *user) {
    appliquer_regles_role(user);

    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) {
        printf("\n  Aucun historique d'emprunt trouve.\n");
        return;
    }

    Emprunt e;
    int trouve = 0;
    time_t maintenant = time(NULL);

    printf("\n--- VOS EMPRUNTS EN COURS ---\n");
    printf("  ID   | TITRE                | TEMPS RESTANT\n");
    printf("  -----|----------------------|--------------\n");

    while (fscanf(f, "%63[^:]:%d:%ld:%d\n",
                  e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            for (int i = 0; i < nb_livres; i++) {
                if (biblio[i].id == e.id_livre) {
                    long ecoule  = (long)difftime(maintenant,
                                   (time_t)e.timestamp_emprunt);
                    long restant = temps_max - ecoule;

                    if (restant > 0) {
                        printf("  %-4d | %-20s | %ld s restante(s)\n",
                               biblio[i].id, biblio[i].titre, restant);
                    } else {
                        printf("  %-4d | %-20s | EN RETARD (%ld s)\n",
                               biblio[i].id, biblio[i].titre, -restant);
                    }
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
    time_t maintenant = time(NULL);

    printf("\n--- VOS RETARDS ---\n");
    printf("  (Limite autorisee : %d seconde(s))\n\n", temps_max);

    while (fscanf(f, "%63[^:]:%d:%ld:%d\n",
                  e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            long ecoule = (long)difftime(maintenant, (time_t)e.timestamp_emprunt);
            if (ecoule > temps_max) {
                for (int i = 0; i < nb_livres; i++) {
                    if (biblio[i].id == e.id_livre) {
                        printf("  [!] EN RETARD : %s\n", biblio[i].titre);
                        printf("      Temps ecoule : %ld s  |  Limite : %d s  |  Retard : %ld s\n\n",
                               ecoule, temps_max, ecoule - temps_max);
                        retards_trouves = 1;
                        break;
                    }
                }
            }
        }
    }

    if (!retards_trouves) {
        printf("  Felicitations, vous n'avez aucun retard.\n");
    }
    fclose(f);
}
