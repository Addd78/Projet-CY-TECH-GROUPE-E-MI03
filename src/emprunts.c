// emprunts.c — Gestion des emprunts et rendus de livres
// Ce fichier contient toutes les fonctions liées au cycle de vie d'un emprunt :
//   - Appliquer les règles selon le rôle de l'utilisateur (élève / professeur)
//   - Compter les emprunts actifs et les retards d'un utilisateur
//   - Vérifier qu'un emprunt est autorisé avant de le valider
//   - Enregistrer un emprunt dans emprunts.txt
//   - Marquer un livre comme rendu (mise à jour du fichier + stock)
//   - Afficher les emprunts en cours et les retards
// Format d'une ligne dans emprunts.txt :
//   login:id_livre:timestamp_emprunt:rendu
//   ex : alice:3:1716540000:0   (0 = non rendu, 1 = rendu)
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../include/utilisateurs.h"
#include "../include/livres.h"
#include "../include/emprunts.h"
// --- Codes couleur ANSI pour le terminal ---
#define ROUGE  "\033[31m"   // Texte rouge (retards, erreurs)
#define RESET  "\033[0m"    // Réinitialise la couleur
// Chemin vers le fichier de persistance des emprunts
#define FICHIER_EMPRUNTS "data/emprunts.txt"
// Variables globales statiques — visibles uniquement dans ce fichier (.c).
// Elles sont mises à jour par appliquer_regles_role() selon le rôle
// de l'utilisateur connecté, avant chaque opération sensible.
static int livre_max;   // Nombre maximum de livres empruntables simultanément
static int temps_max;   // Durée maximale d'un emprunt en secondes
// appliquer_regles_role — Configure les limites selon le rôle de l'utilisateur
// Paramètre :
//   personne — pointeur vers l'utilisateur dont on veut appliquer les règles
// Règles :
//   - PROFESSEUR : jusqu'à 5 livres, 180 secondes de délai
//   - ELEVE      : jusqu'à 3 livres, 120 secondes de délai
// Cette fonction doit être appelée en début de chaque opération qui dépend
// des limites (emprunt, retard, affichage).
void appliquer_regles_role(Utilisateur *personne) {
    if (personne->role == PROFESSEUR) {
        livre_max = 5;
        temps_max = 180;
    } else {
        // Par défaut : rôle ELEVE
        livre_max = 3;
        temps_max = 120;
    }
}
// compter_livres_empruntes — Compte les emprunts actifs d'un utilisateur
// Parcourt emprunts.txt ligne par ligne et compte toutes les entrées
// correspondant au login de l'utilisateur avec rendu == 0 (non rendu).
// Paramètre :
//   user — pointeur vers l'utilisateur concerné
// Retourne :
//   Le nombre de livres actuellement empruntés (non rendus), ou 0 si le
//   fichier est inaccessible.
int compter_livres_empruntes(Utilisateur *user) {
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) return 0;   // Aucun emprunt si le fichier n'existe pas encore

    Emprunt e;
    int count = 0;

    // Lecture ligne par ligne au format : login:id_livre:timestamp:rendu
    while (fscanf(f, "%20[^:]:%d:%ld:%d\n", e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        // On ne compte que les emprunts de cet utilisateur qui ne sont pas encore rendus
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            count++;
        }
    }

    fclose(f);
    return count;
}
// compter_retards — Compte les emprunts en retard d'un utilisateur
// Un emprunt est considéré en retard si le temps écoulé depuis le timestamp
// d'emprunt dépasse temps_max (en secondes), et que le livre n'est pas rendu.
// Paramètre :
//   user — pointeur vers l'utilisateur concerné
// Retourne :
//   Le nombre d'emprunts en retard, ou 0 si le fichier est inaccessible.
int compter_retards(Utilisateur *user) {
    // Mettre à jour les limites selon le rôle avant de calculer les retards
    appliquer_regles_role(user);
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) return 0;
    Emprunt e;
    int count = 0;
    time_t maintenant = time(NULL);   // Timestamp Unix actuel (secondes depuis 1970)
    while (fscanf(f, "%20[^:]:%d:%ld:%d\n", e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            // Calcul du nombre de secondes écoulées depuis l'emprunt
            long secondes_ecoules = (long)difftime(maintenant, (time_t)e.timestamp_emprunt);
            // Si ce délai dépasse la limite autorisée, c'est un retard
            if (secondes_ecoules > temps_max) {
                count++;
            }
        }
    }
    fclose(f);
    return count;
}
// verif_emprunt — Vérifie si un emprunt est autorisé
// Effectue trois vérifications dans l'ordre :
//   1. Le livre est-il disponible en rayon ?
//   2. L'utilisateur a-t-il des livres en retard ?
//   3. L'utilisateur a-t-il atteint son quota d'emprunts ?
// Paramètres :
//   personne          — l'utilisateur qui souhaite emprunter
//   livres_empruntes  — nombre de livres actuellement empruntés (calculé avant l'appel)
//   nb_livres_retard  — nombre de livres en retard (calculé avant l'appel)
//   l                 — pointeur vers le livre convoité
// Retourne :
//   1 si l'emprunt est autorisé, 0 sinon (avec message d'erreur affiché).
int verif_emprunt(Utilisateur *personne, int livres_empruntes, int nb_livres_retard, Livre *l) {
    // Mise à jour des limites selon le rôle de l'utilisateur
    appliquer_regles_role(personne);
    // Vérification 1 : au moins un exemplaire doit être disponible en rayon
    if (l->quantite_disponible <= 0) {
        printf("Erreur : plus aucun exemplaire disponible.\n");
        return 0;
    }
    // Vérification 2 : aucun retard toléré — l'utilisateur doit d'abord rendre ses livres en retard
    if (nb_livres_retard > 0) {
        printf("Erreur : vous avez %d livre(s) en retard.\n", nb_livres_retard);
        printf("Rendez tous vos livres en retard avant d'emprunter un autre.\n");
        return 0;
    }
    // Vérification 3 : quota d'emprunts simultanés non atteint
    if (livres_empruntes >= livre_max) {
        printf("Erreur : vous avez atteint la limite de %d livre(s).\n", livre_max);
        return 0;
    }
    // Toutes les conditions sont réunies : emprunt autorisé
    printf("Emprunt autorise ! (%d/%d livres)\n", livres_empruntes + 1, livre_max);
    return 1;
}
// emprunter_livre — Permet à un utilisateur d'emprunter un livre
// Déroulement :
//   1. Saisie de l'ID du livre par l'utilisateur
//   2. Recherche du livre dans le catalogue (biblio[])
//   3. Calcul des compteurs réels depuis emprunts.txt
//   4. Vérification des conditions via verif_emprunt()
//   5. Si autorisé : décrémentation du stock, sauvegarde, enregistrement
//      de l'emprunt dans emprunts.txt avec le timestamp actuel et rendu=0
// Paramètres :
//   biblio    — tableau de tous les livres du catalogue
//   nb_livres — nombre de livres dans le catalogue
//   user      — utilisateur qui effectue l'emprunt
void emprunter_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;   // Indice du livre dans biblio[], -1 = non trouvé
    printf("\n### EMPRUNTER UN LIVRE ###\n");
    printf("Entrez l'ID du livre : ");
    // Saisie sécurisée de l'ID (entier attendu)
    if (scanf("%d", &id_voulu) != 1) {
        printf("Erreur de saisie.\n");
        while (getchar() != '\n');   // Vider le buffer de saisie
        return;
    }
    while (getchar() != '\n');   // Consommer le '\n' laissé par scanf
    // Recherche linéaire du livre par son ID dans le catalogue
    for (int i = 0; i < nb_livres; i++) {
        if (biblio[i].id == id_voulu) {
            index_livre = i;
            break;
        }
    }
    // ID introuvable dans le catalogue
    if (index_livre == -1) {
        printf("[!] Livre ID %d introuvable.\n", id_voulu);
        return;
    }
    // Calcul des compteurs réels depuis emprunts.txt (ne pas se fier à une variable globale)
    int emprunts_actuels = compter_livres_empruntes(user);
    int retards_actuels  = compter_retards(user);
    // Vérification des conditions d'emprunt
    if (verif_emprunt(user, emprunts_actuels, retards_actuels, &biblio[index_livre])) {
        // Décrémentation du stock disponible et sauvegarde immédiate dans livres.txt
        biblio[index_livre].quantite_disponible--;
        sauvegarder_livres(biblio, nb_livres);
        // Enregistrement de l'emprunt dans emprunts.txt en mode ajout ("a")
        // Ligne ajoutée : login:id_livre:timestamp:0  (0 = non rendu)
        FILE *fe = fopen(FICHIER_EMPRUNTS, "a");
        if (fe != NULL) {
            long ts = (long)time(NULL);   // Timestamp Unix de l'emprunt
            fprintf(fe, "%s:%d:%ld:0\n", user->login, biblio[index_livre].id, ts);
            fclose(fe);
        }
        printf("Succes ! Vous avez emprunte : %s\n", biblio[index_livre].titre);
        printf("Il reste %d exemplaire(s) en rayon.\n", biblio[index_livre].quantite_disponible);
        // Rappel de la durée maximale selon le rôle
        appliquer_regles_role(user);
        printf("Vous devez le rendre dans %d seconde(s).\n", temps_max);
    }
}
// rendre_livre — Permet à un utilisateur de rendre un livre emprunté
// Stratégie de mise à jour du fichier emprunts.txt :
//   Comme on ne peut pas modifier une ligne en place dans un fichier texte,
//   on utilise un fichier temporaire (emprunts_tmp.txt) :
//     1. On recopie toutes les lignes dans le fichier temporaire.
//     2. Pour la première ligne correspondant à cet utilisateur + ce livre
//        avec rendu == 0, on écrit rendu = 1 à la place.
//     3. On supprime l'ancien fichier et on renomme le temporaire.
//   Le stock et l'affichage du succès ne sont mis à jour QUE si la ligne
//   a bien été trouvée et modifiée (updated == 1).
// Paramètres :
//   biblio    — tableau de tous les livres du catalogue
//   nb_livres — nombre de livres dans le catalogue
//   user      — utilisateur qui rend le livre
void rendre_livre(Livre biblio[], int nb_livres, Utilisateur *user) {
    int id_voulu;
    int index_livre = -1;
    printf("\n#### RENDRE UN LIVRE ####\n");
    printf("Entrez l'ID du livre a rendre : ");
    // Saisie sécurisée de l'ID
    if (scanf("%d", &id_voulu) != 1) {
        printf("Erreur de saisie.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    // Recherche du livre dans le catalogue
    for (int i = 0; i < nb_livres; i++) {
        if (biblio[i].id == id_voulu) {
            index_livre = i;
            break;
        }
    }
    // ID introuvable dans le catalogue
    if (index_livre == -1) {
        printf("[!] Livre ID %d inconnu du catalogue.\n", id_voulu);
        return;
    }
    // Garde-fou : si tous les exemplaires sont déjà en rayon, personne ne peut
    // en rendre un de plus (incohérence de données)
    if (biblio[index_livre].quantite_disponible >= biblio[index_livre].quantite_totale) {
        printf("[!] Tous les exemplaires de '%s' sont deja en rayon.\n",
               biblio[index_livre].titre);
        return;
    }
    // --- Mise à jour de emprunts.txt via fichier temporaire ---
    // Ouverture du fichier source en lecture
    FILE *f_in = fopen(FICHIER_EMPRUNTS, "r");
    if (f_in == NULL) {
        printf("[!] Impossible d'acceder au fichier des emprunts.\n");
        return;
    }
    // Ouverture du fichier temporaire en écriture
    FILE *f_out = fopen("data/emprunts_tmp.txt", "w");
    if (f_out == NULL) {
        printf("[!] Impossible de creer le fichier temporaire.\n");
        fclose(f_in);   // On ferme f_in avant de quitter pour éviter une fuite de ressource
        return;
    }
    Emprunt e;
    int updated = 0;   // Indique si on a bien trouvé et modifié un emprunt actif
    // Recopie de toutes les lignes, en modifiant uniquement la première
    // qui correspond à : même login + même id_livre + non rendu (rendu == 0)
    while (fscanf(f_in, "%20[^:]:%d:%ld:%d\n", e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (!updated && strcmp(e.login, user->login) == 0 && e.id_livre == id_voulu && e.rendu == 0) {
            e.rendu = 1;   // On marque cet emprunt comme rendu
            updated = 1;   // On ne modifie qu'un seul emprunt (le premier trouvé)
        }
        // Réécriture de la ligne (modifiée ou non) dans le fichier temporaire
        fprintf(f_out, "%s:%d:%ld:%d\n", e.login, e.id_livre, e.timestamp_emprunt, e.rendu);
    }
    // Fermeture des deux fichiers avant toute opération sur le système de fichiers
    fclose(f_out);
    fclose(f_in);
    // Si aucun emprunt actif n'a été trouvé, on annule : on supprime le temporaire
    // et on n'incrémente PAS le stock (le livre n'était pas réellement emprunté)
    if (!updated) {
        printf("[!] Aucun emprunt actif trouve pour ce livre.\n");
        remove("data/emprunts_tmp.txt");
        return;
    }
    // Remplacement atomique : suppression de l'ancien fichier, renommage du temporaire
    remove(FICHIER_EMPRUNTS);
    rename("data/emprunts_tmp.txt", FICHIER_EMPRUNTS);
    // Mise à jour du stock uniquement si le rendu a bien été enregistré dans le fichier
    biblio[index_livre].quantite_disponible++;
    sauvegarder_livres(biblio, nb_livres);
    printf("[V] Succes ! Vous avez rendu : %s\n", biblio[index_livre].titre);
    printf("Stock disponible : %d/%d\n", biblio[index_livre].quantite_disponible, biblio[index_livre].quantite_totale);
}
// afficher_emprunts_utilisateur — Affiche les emprunts en cours d'un utilisateur
// Lit emprunts.txt et affiche sous forme de tableau tous les emprunts non rendus
// de l'utilisateur, avec pour chaque livre :
//   - son ID et son titre (recherché dans biblio[])
//   - le temps restant avant la limite (en vert implicite si dans les temps,
//     en rouge si en retard)
// Paramètres :
//   biblio    — tableau de tous les livres (pour retrouver les titres)
//   nb_livres — nombre de livres dans le catalogue
//   user      — utilisateur dont on affiche les emprunts
void afficher_emprunts_utilisateur(Livre biblio[], int nb_livres, Utilisateur *user) {
    // Mise à jour de temps_max selon le rôle avant le calcul des délais
    appliquer_regles_role(user);
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) {
        printf("\n  Aucun historique d'emprunt trouve.\n");
        return;
    }
    Emprunt e;
    int trouve = 0;                    // Au moins un emprunt affiché ?
    time_t maintenant = time(NULL);    // Timestamp actuel pour calculer le temps restant
    printf("\n--- VOS EMPRUNTS EN COURS ---\n");
    printf("ID   | TITRE                | TEMPS RESTANT\n");
    printf("-----|----------------------|--------------\n");
    while (fscanf(f, "%20[^:]:%d:%ld:%d\n", e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        // On ne traite que les emprunts actifs (non rendus) de cet utilisateur
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            // Recherche du titre du livre dans le catalogue
            for (int i = 0; i < nb_livres; i++) {
                if (biblio[i].id == e.id_livre) {
                    long ecoule  = (long)difftime(maintenant, (time_t)e.timestamp_emprunt);
                    long restant = temps_max - ecoule;   // Peut être négatif si en retard
                    if (restant > 0) {
                        // Emprunt dans les temps : affichage normal
                        printf("%-4d | %-20s | %ld s restante(s)\n", biblio[i].id, biblio[i].titre, restant);
                    } else {
                        // Emprunt en retard : affichage en rouge avec le dépassement
                        printf(ROUGE "%-4d | %-20s | EN RETARD (%ld s)" RESET "\n",
                               biblio[i].id, biblio[i].titre, -restant);
                    }
                    trouve = 1;
                    break;   // On a trouvé le livre, inutile de continuer la boucle interne
                }
            }
        }
    }
    if (!trouve) {
        printf("Vous n'avez aucun emprunt en cours.\n");
    }
    printf("-------------------------------------------\n");
    fclose(f);
}
// afficher_retards — Affiche uniquement les emprunts en retard d'un utilisateur
// Parcourt emprunts.txt et affiche (en rouge) les livres non rendus dont le
// délai d'emprunt a dépassé la limite autorisée (temps_max).
// Pour chaque retard, affiche le temps écoulé, la limite et le dépassement.
// Paramètres :
//   biblio    — tableau de tous les livres (pour retrouver les titres)
//   nb_livres — nombre de livres dans le catalogue
//   user      — utilisateur dont on vérifie les retards
void afficher_retards(Livre biblio[], int nb_livres, Utilisateur *user) {
    // Mise à jour de temps_max selon le rôle avant toute comparaison
    appliquer_regles_role(user);
    FILE *f = fopen(FICHIER_EMPRUNTS, "r");
    if (f == NULL) {
        printf("\n[!] Impossible d'acceder aux donnees d'emprunts.\n");
        return;
    }
    Emprunt e;
    int retards_trouves = 0;           // Au moins un retard trouvé ?
    time_t maintenant = time(NULL);    // Timestamp actuel
    printf("\n--- VOS RETARDS ---\n");
    printf("(Limite autorisee : %d seconde(s))\n\n", temps_max);
    while (fscanf(f, "%20[^:]:%d:%ld:%d\n", e.login, &e.id_livre, &e.timestamp_emprunt, &e.rendu) == 4) {
        if (strcmp(e.login, user->login) == 0 && e.rendu == 0) {
            long ecoule = (long)difftime(maintenant, (time_t)e.timestamp_emprunt);
            // Un retard se produit uniquement si le délai maximal est dépassé
            if (ecoule > temps_max) {
                // Recherche du titre dans le catalogue pour l'affichage
                for (int i = 0; i < nb_livres; i++) {
                    if (biblio[i].id == e.id_livre) {
                        printf(ROUGE "[!] EN RETARD : %s" RESET "\n", biblio[i].titre);
                        printf(ROUGE "Temps ecoule : %ld s  |  Limite : %d s  |  Retard : %ld s" RESET "\n\n",
                               ecoule, temps_max, ecoule - temps_max);
                        retards_trouves = 1;
                        break;
                    }
                }
            }
        }
    }
    if (!retards_trouves) {
        printf("Felicitations, vous n'avez aucun retard.\n");
    }
    fclose(f);
}
