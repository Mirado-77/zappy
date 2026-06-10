##
## EPITECH PROJECT, 2026
## Zappy
## File description:
## Protocol library Makefile
##

NAME = libzappy_protocol.a

ROOT = ../..
BUILD_DIR = $(ROOT)/build/protocol
OBJ_DIR = $(BUILD_DIR)/obj

SRC_EXTENSION = .cpp
SRC = $(wildcard src/*$(SRC_EXTENSION)) \
      $(wildcard src/**/*$(SRC_EXTENSION))
OBJ = $(patsubst src/%$(SRC_EXTENSION),$(OBJ_DIR)/%.o,$(SRC))

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror
CXXFLAGS += -I include/

AR = ar
ARFLAGS = rcs

TEST_BIN = unit_tests_protocol
TEST_SRC = $(wildcard tests/*$(SRC_EXTENSION))
TEST_CXXFLAGS = -std=c++20 -Wall -Wextra -I include/
TEST_LDLIBS = -lcriterion

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJ_DIR)/%.o: src/%$(SRC_EXTENSION)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests_run: $(NAME)
	$(CXX) $(TEST_CXXFLAGS) $(TEST_SRC) -L$(CURDIR) -lzappy_protocol $(TEST_LDLIBS) -o $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f $(TEST_BIN)

re: fclean all

.PHONY: all tests_run clean fclean re
