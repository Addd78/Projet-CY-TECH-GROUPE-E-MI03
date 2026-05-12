# Nom de l'exécutable final
TARGET = cy_bibliotech

# Compilateur
CC = gcc

# Options de compilation
# -Wall -Wextra : affiche un maximum d'avertissements pour coder proprement
# -Iinclude : indique au compilateur de chercher les headers dans le dossier include
CFLAGS = -Wall -Wextra -Iinclude

# Dossiers
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Liste des fichiers sources
SRC = $(wildcard $(SRC_DIR)/*.c)

# Liste des fichiers objets
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# --- RÈGLES ---

# Règle par défaut : compile le projet
all: $(TARGET)

# Liaison des fichiers objets pour créer l'exécutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "--------------------------------------"
	@echo " Compilation terminée : ./$(TARGET)"
	@echo "--------------------------------------"

# Compilation des fichiers .c en .o 
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création du dossier obj s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Nettoyage des fichiers temporaires
clean:
	rm -rf $(OBJ_DIR)
	@echo "Fichiers objets supprimés."

# Nettoyage complet (objets + exécutable)
fclean: clean
	rm -f $(TARGET)
	@echo "Exécutable supprimé."

# Recompiler de zéro
re: fclean all

# Indique que ces règles ne sont pas des fichiers
.PHONY: all clean fclean re