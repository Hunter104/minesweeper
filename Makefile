CXX=g++
CXXFLAGS=-Wall -Wextra -Werror
DEBUGFLAGS=-ggdb3 -DDEBUG

BUILD_DIR=build
SRC_DIR=src
TEST_DATA_DIR=test-data
TEST_FILE=$(TEST_DATA_DIR)/input

MAIN_FILE=$(SRC_DIR)/main.cpp

EXECUTABLE=minesweeper
RM=rm -f

SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
SRC_FILES_EXCLUDING_MAIN=$(filter-out $(MAIN_FILE), $(SRC_FILES))


all: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(EXECUTABLE): $(MAIN_FILE) $(SRC_FILES_EXCLUDING_MAIN)
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "Generating single source file..."
	$(eval TMP_MOJ := $(shell mktemp -t "MOJ.XXXXXX"))
	./expand $(MAIN_FILE) > $(TMP_MOJ)
	@echo "Cleaning up includes..."
	{ grep '^#include' $(TMP_MOJ) | sort -u; grep -v '^#include' $(TMP_MOJ); } > $(BUILD_DIR)/moj.cpp
	rm -f $(TMP_MOJ)
	clang-format -i $(BUILD_DIR)/moj.cpp

clean:
	$(RM) -r $(BUILD_DIR)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: all 

run: $(BUILD_DIR)/$(EXECUTABLE)
	@echo "Running with file $(TEST_FILE)"
	./$(BUILD_DIR)/$(EXECUTABLE)

.PHONY: all clean debug
