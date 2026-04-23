#include <stdio.h>

typedef enum{
professeur,
etudiant
}Role;

typedef struct{
Role role;

}utilisateur;



int livre_max;
int temps_max;






int choix(utilisateur*personne){
if(personne->role==professeur){
livre_max=5;
temps_max=180;
}
else{
    livre_max=3;
    temps_max=120;
    } 

}  

int verif_emprunt(utilisateur*personne,int temps_actuel,int livres_emprunts){
    choix (personne,int nb_livres_retard);
    
    if(livres_emprunts>livre_max){
        printf("limite de livres depassé: %d/%d",livres_emprunts,livre_max);
        return 0;
    }
      
    if(nb_livres_retard>0){
        printf("vous avez %d livres de retard,rendez les !",nb_livres_retard);
        return 1;
      
        
        
    if(temps_actuel>temps_max){
        printf("temps d'emprunt depassé:aucun emprunt possible  %d/%d",temps_actuel,temps_max);
        return 0}
        
        printf("emprunt autorisé");
        return 1;
        
}
        
        
        
        
        
        
        
        )
    
}
