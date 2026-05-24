Bienvenue dans le projet CY-biblioTECH

Réalisé par Guillaume CANAUD, Adel CHAOUCHI et Sayfeddine Kadraoui

CY-biblioTECH est un gestionnaire de bibliothèque universitaire codé en C, qui tourne directement dans le terminal. L'idée principale est de proposer une application capable de différencier automatiquement un Étudiant d'un Professeur pour lui appliquer des droits d'accès et des règles de prêt spécifiques.

Pour que le projet reste propre, le code est découpé de manière logique : les fichiers .c vont dans src, les fichiers .h dans include, et toutes les données sont sauvegardées dans des fichiers texte au sein du dossier data. Un Makefile sur mesure s'occupe d'automatiser la compilation sous les drapeaux stricts -Wall -Wextra pour traquer la moindre erreur, tout en isolant les fichiers objets dans un sous-dossier pour ne pas polluer l'espace de travail.

Côté fonctionnalités, l'application gère tout le cycle de la bibliothèque. Au démarrage, on peut s'inscrire ou se connecter via une gestion de session sécurisée qui vérifie l'unicité des logins. Une fois en ligne, l'utilisateur accède à un catalogue dynamique. Il peut parcourir l'ensemble des livres ou utiliser un moteur de recherche par mots-clés qui scanne simultanément les titres, auteurs et catégories. Les chaînes de caractères et les tampons de saisie (scanf, fgets) sont systématiquement nettoyés pour éviter les bugs de mémoire et les plantages textuels.

La vraie force du programme réside dans son gestionnaire de règles métiers automatique. Dès qu'un utilisateur veut emprunter, le système vérifie son rôle :

- Étudiant : Limité à un maximum de 3 livres simultanés pour une durée de 120 secondes maximum. Il dispose uniquement des droits de consultation et d'emprunt.

- Professeur : Privilèges étendus avec un plafond de 5 livres sur 180 secondes, complété par le droit exclusif d'ajouter de nouvelles références physiques au catalogue.

Chaque transaction valide (emprunt ou retour) recalcule instantanément l'état des stocks disponibles et met à jour les fichiers de persistance. Enfin, un module d'alerte permet à chacun de vérifier l'état de ses emprunts en cours et de lister ses retards potentiels par rapport aux limites fixées par son statut.

Pour faire fonctionner l'application :
  - Un compilateur C comme GCC
  - L'utilitaire Makefile

Pour le compiler :
  - Ouvrir un terminal à la racine du dossier de l'application
  - Taper "make" puis Entrée

Pour l'exécuter :
  - Taper "./cy_bibliotech" puis Entrée

Pour Naviguer dans l'application :
    Dans les menus vous trouvez des actions permettant de naviguer et pour les actionner vous devez taper le chiffre present sur la gauche de celles-ci.
    Pour sélectionner un livre vous devez taper l'ID de celui-ci, qui se situe entre 1 et 200, et pour trouver l'ID du livre que vous cherchez, il se trouve dans la liste des livres (tout à gauche).

Pour nettoyer les fichiers objets :
  - Taper "make clean" puis Entrée

Pour recompiler le programme :
  - Taper "make re" puis Entrée


