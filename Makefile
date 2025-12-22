# Scradle Engine Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iengine/include -Iengine/tests
LDFLAGS =

# Directories
SRC_DIR = engine/src
INC_DIR = engine/include
OBJ_DIR = engine/build
BIN_DIR = bin
TEST_DIR = engine/tests

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Targets
TEST_BOARD_TARGET = $(BIN_DIR)/test_board
TEST_DAWG_TARGET = $(BIN_DIR)/test_dawg
TEST_MOVEGEN_TARGET = $(BIN_DIR)/test_move_generator
TEST_SCORER_TARGET = $(BIN_DIR)/test_scorer
TEST_BLANKS_TARGET = $(BIN_DIR)/test_blanks
TEST_INTEGRATION_TARGET = $(BIN_DIR)/test_integration
TEST_COMPLEX_TARGET = $(BIN_DIR)/test_complex_board
TEST_TILE_BAG_TARGET = $(BIN_DIR)/test_tile_bag
TEST_GAME_STATE_TARGET = $(BIN_DIR)/test_game_state
TEST_DUPLICATE_GAME_TARGET = $(BIN_DIR)/test_duplicate_game

.PHONY: all clean test test-board test-dawg test-movegen test-scorer test-blanks test-integration test-complex test-tile-bag test-game-state test-duplicate-game test-all dirs

all: dirs $(OBJECTS)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test-board: dirs $(TEST_BOARD_TARGET)
	./$(TEST_BOARD_TARGET)

test-dawg: dirs $(TEST_DAWG_TARGET)
	./$(TEST_DAWG_TARGET)

test-movegen: dirs $(TEST_MOVEGEN_TARGET)
	./$(TEST_MOVEGEN_TARGET)

test-scorer: dirs $(TEST_SCORER_TARGET)
	./$(TEST_SCORER_TARGET)

test-blanks: dirs $(TEST_BLANKS_TARGET)
	./$(TEST_BLANKS_TARGET)

test-integration: dirs $(TEST_INTEGRATION_TARGET)
	./$(TEST_INTEGRATION_TARGET)

test-complex: dirs $(TEST_COMPLEX_TARGET)
	./$(TEST_COMPLEX_TARGET)

test-tile-bag: dirs $(TEST_TILE_BAG_TARGET)
	./$(TEST_TILE_BAG_TARGET)

test-game-state: dirs $(TEST_GAME_STATE_TARGET)
	./$(TEST_GAME_STATE_TARGET)

test-duplicate-game: dirs $(TEST_DUPLICATE_GAME_TARGET)
	./$(TEST_DUPLICATE_GAME_TARGET)

test-all: test-board test-dawg test-movegen test-scorer test-blanks test-integration test-complex test-tile-bag test-game-state test-duplicate-game

$(TEST_BOARD_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_main.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_main.cpp -o $@

$(TEST_DAWG_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_dawg.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_dawg.cpp -o $@

$(TEST_MOVEGEN_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_move_generator.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_move_generator.cpp -o $@

$(TEST_SCORER_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_scorer.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_scorer.cpp -o $@

$(TEST_BLANKS_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_blanks.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_blanks.cpp -o $@

$(TEST_INTEGRATION_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_integration.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_integration.cpp -o $@

$(TEST_COMPLEX_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_complex_board.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_complex_board.cpp -o $@

$(TEST_TILE_BAG_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_tile_bag.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_tile_bag.cpp -o $@

$(TEST_GAME_STATE_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_game_state.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_game_state.cpp -o $@

$(TEST_DUPLICATE_GAME_TARGET): $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_duplicate_game.cpp
	$(CXX) $(CXXFLAGS) $(filter-out $(OBJ_DIR)/main.o,$(OBJECTS)) $(TEST_DIR)/test_duplicate_game.cpp -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

clean-test: clean test-all

help:
	@echo "Scradle Engine Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  make                 - Build the engine"
	@echo "  make test            - Build and run board/rack tests (default test)"
	@echo "  make test-board      - Build and run board/rack tests"
	@echo "  make test-dawg       - Build and run DAWG tests"
	@echo "  make test-movegen    - Build and run move generator tests"
	@echo "  make test-scorer     - Build and run scoring tests"
	@echo "  make test-blanks     - Build and run blank tile tests"
	@echo "  make test-integration- Build and run integration tests (real game)"
	@echo "  make test-complex    - Build and run complex board tests (custom scenarios)"
	@echo "  make test-all        - Run all tests"
	@echo "  make clean           - Remove build artifacts"
	@echo "  make help            - Show this help message"
