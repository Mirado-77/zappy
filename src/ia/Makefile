##
## EPITECH PROJECT, 2026
## Zappy
## File description:
## AI Makefile
##

NAME		=	zappy_ai

ROOT		=	../..
BIN_DIR		=	$(ROOT)
BUILD_DIR	=	$(ROOT)/build/ia

SRC			=	main.py

all:	$(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME):	$(SRC)
	@mkdir -p $(BIN_DIR) $(BUILD_DIR)
	cp $(SRC) $(BIN_DIR)/$(NAME)
	chmod +x $(BIN_DIR)/$(NAME)

debug:	all

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(BIN_DIR)/$(NAME)

re: fclean all

debugre: fclean debug

.PHONY: all debug clean fclean re debugre
