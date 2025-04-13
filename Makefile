# Para build unity só é preciso compilar um arquivo
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

clean:
	$(RM) -r $(BUILD_DIR)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: all 

run: $(BUILD_DIR)/$(EXECUTABLE)
	@echo "Running with file $(TEST_FILE)"
	./$(BUILD_DIR)/$(EXECUTABLE) < $(TEST_FILE)

.PHONY: all clean debug
