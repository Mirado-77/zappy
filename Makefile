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

COV_FLAGS = --coverage
TEST_SRC = src/server/posix/FileDescriptor.cpp \
			src/server/posix/Address.cpp \
			src/server/network/ClientBuffer.cpp \
			src/server/network/Listener.cpp \
			src/server/network/SignalHandler.cpp \
			src/server/network/PollLoop.cpp \
      		src/server/cli/CliParser.cpp \
			src/server/Server.cpp \
			src/server/client/Client.cpp \
			src/server/client/ClientRegistry.cpp \
		    $(wildcard tests/cli/*.cpp) \
			$(wildcard tests/server/posix/*.cpp) \
			$(wildcard tests/server/network/*.cpp) \
			$(wildcard tests/server/client/*.cpp)

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

	rm -f *.gcno *.gcda *.gcov

tests_run:
	$(TEST_CXX) $(TEST_CXXFLAGS) $(TEST_SRC) -o $(TEST_BIN) $(TEST_LDLIBS)
	./$(TEST_BIN)

tests_cov: TEST_CXXFLAGS += $(COV_FLAGS)
tests_cov: TEST_LDLIBS += $(COV_FLAGS)
tests_cov: tests_run
	gcovr --exclude tests/ --print-summary

re: fclean all

debugre: fclean debug

.PHONY: all debug clean fclean re debugre tests_run tests_cov $(SERVER) $(GUI) $(AI)