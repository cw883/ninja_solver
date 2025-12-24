CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Directories
GAME_DIR = game
TEST_DIR = tests
BUILD_DIR = build

# Source files
GAME_SRCS = $(wildcard $(GAME_DIR)/*.cpp)
GAME_OBJS = $(patsubst $(GAME_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(GAME_SRCS))

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_BINS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%,$(TEST_SRCS))

# Default target
all: $(BUILD_DIR) $(TEST_BINS)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile game source files to object files
$(BUILD_DIR)/%.o: $(GAME_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test executables
$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(GAME_OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< $(GAME_OBJS) -o $@

# Run all tests
test: all
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Rebuild everything
rebuild: clean all

.PHONY: all test clean rebuild
