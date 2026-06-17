##
## EPITECH PROJECT, 2026
## Zappy
## File description:
## GUI Makefile
##

NAME = zappy_gui

ROOT = ../..
BIN_DIR = $(ROOT)
BUILD_DIR = $(ROOT)/build/gui
OBJ_DIR = $(BUILD_DIR)/obj
DBG_DIR = $(BUILD_DIR)/debug
SRC_EXTENSION = .cpp

SRC = $(wildcard *$(SRC_EXTENSION)) \
		$(wildcard **/*$(SRC_EXTENSION)) \
		$(wildcard **/**/*$(SRC_EXTENSION))

OBJ = $(SRC:%$(SRC_EXTENSION)=$(OBJ_DIR)/%.o)
DEBUG_OBJ = $(SRC:%$(SRC_EXTENSION)=$(DBG_DIR)/%.debug.o)

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
CXXFLAGS += -iquote $(ROOT)/include/gui/

DEBUG_CFLAGS = -g3 -fsanitize=address -DDEBUG
DEBUG_LDFLAGS = -fsanitize=address

all:	$(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME):	$(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS) -lraylib

debug: CXXFLAGS += $(DEBUG_CFLAGS)
debug: $(BIN_DIR)/$(NAME)-debug

$(BIN_DIR)/$(NAME)-debug:	$(DEBUG_OBJ)
	$(CXX) -o $(BIN_DIR)/$(NAME) $^ $(LDFLAGS) $(DEBUG_LDFLAGS)

$(OBJ_DIR)/%.o:	%$(SRC_EXTENSION)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DBG_DIR)/%.debug.o:	%$(SRC_EXTENSION)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(BIN_DIR)/$(NAME)

re: fclean all

debugre: fclean debug

.PHONY: all debug clean fclean re debugre
