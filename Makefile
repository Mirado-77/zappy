##
## EPITECH PROJECT, 2026
## Zappy
## File description:
## Root Makefile
##

SERVER = zappy_server
GUI = zappy_gui
AI = zappy_ai

BINS = $(SERVER) $(GUI) $(AI)

SERVER_DIR = src/server
GUI_DIR = src/gui
AI_DIR = src/ia

BUILD_DIR = build

TEST_BIN = unit_tests
TEST_CXX = g++
TEST_CXXFLAGS = -std=c++20 -Wall -Wextra -iquote include/
TEST_LDLIBS = -lcriterion
TEST_SRC = src/server/cli/CliParser.cpp $(wildcard tests/cli/*.cpp)

all:	$(BINS)

$(SERVER):
	@$(MAKE) -C $(SERVER_DIR)

$(GUI):
	@$(MAKE) -C $(GUI_DIR)

$(AI):
	@$(MAKE) -C $(AI_DIR)

debug:
	@$(MAKE) -C $(SERVER_DIR) debug
	@$(MAKE) -C $(GUI_DIR) debug
	@$(MAKE) -C $(AI_DIR) debug

clean:
	@$(MAKE) -C $(SERVER_DIR) clean
	@$(MAKE) -C $(GUI_DIR) clean
	@$(MAKE) -C $(AI_DIR) clean
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f $(BINS)
	rm -f $(TEST_BIN)

tests_run:
	$(TEST_CXX) $(TEST_CXXFLAGS) $(TEST_SRC) -o $(TEST_BIN) $(TEST_LDLIBS)
	./$(TEST_BIN)

re: fclean all

debugre: fclean debug

.PHONY: all debug clean fclean re debugre tests_run $(SERVER) $(GUI) $(AI)
