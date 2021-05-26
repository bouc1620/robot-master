CXX ?= g++
CXXFLAGS = -std=c++17 -fconstexpr-ops-limit=1000000000

RELEASE ?= 0
ifeq ($(RELEASE), 1)
	CXXFLAGS += -O3
else
	CXXFLAGS += -Ofast
endif

TARGET_NAME = galvaude

SRC_DIR = ./src
OBJ_DIR = ./obj
SRC_FILES := $(shell find $(SRC_DIR) -name *.cpp)
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_FILES:.cpp=.o)))

run: $(TARGET_NAME)
	./$(TARGET_NAME)

all: $(TARGET_NAME)

$(TARGET_NAME): $(OBJ_DIR) $(OBJ_FILES)
	$(CXX) $(LD_FLAGS) -o $(TARGET_NAME) $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(TARGET_NAME)
	$(RM) $(OBJ_DIR)/*

.PHONY: all run clean
