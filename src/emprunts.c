// verifier s'il reste des exemplaires en stock et si l'utilisateur peut emprunter (si etudiant ou prof)
include <stdio.h>
 
typedef enum {
    professeur,
    etudiant
} Role;
 
typedef struct {
    Role role;
    int  peut_emprunter;
} utilisateur;
 
typedef struct {
    int exemplaires_restants;
} livre;
 
int livre_max;
int temps_max;            // voir si possible de faire tourner avec les 2 max ds la struct livre 
 
void choix(utilisateur *personne) {
    if (personne->role == professeur) {
        livre_max = 5;
        temps_max = 180;
    } else {
        livre_max = 3;
        temps_max = 120;
    }
}
 
int verif_emprunt(utilisateur *personne, int temps_actuel,
                  int livres_empruntes, int nb_livres_retard,
                  livre *l) {
 
    choix(personne);
 
    if (personne->peut_emprunter == 0) {
        printf("Refus : votre compte ne permet pas d'emprunter.\n");
        return 0;
    }
 
    if (l->exemplaires_restants <= 0) {    //a le droit d'emprunter mais livre en rupture
        printf("Refus : plus aucun exemplaire disponible.\n");
        return 0;
    }
 
    if (nb_livres_retard > 0) {
        printf("Refus : vous avez %d livre(s) en retard, rendez-les d'abord !\n", nb_livres_retard);
        return 0;
    }
 
    if (livres_empruntes >= livre_max) {
        printf("Refus : limite de livres depassee (%d/%d).\n", livres_empruntes, livre_max);
        return 0;
    }
 
    if (temps_actuel > temps_max) {
        printf("Refus : temps depasse (%ds/%ds).\n", temps_actuel, temps_max);
        return 0;
    }
 
    printf("Emprunt autorise ! (%d/%d livres)\n", livres_empruntes + 1, livre_max);
    return 1;
}
