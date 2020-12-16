##### CONSTANTS #####

# Compilers
GXX=g++
GXX_ARGS=-std=c++17 -O2 -Wall -Wextra
GXX_LINK=-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Folders
SRC	=src
LIB	=$(SRC)/lib
BIN	=bin
OBJ	=$(BIN)/obj
TEST=tests/
VULKANTEST=$(SRC)/VulkanTest
HELLOTRIANGLE=$(SRC)/HelloTriangle



##### INPUT #####
ifdef DEBUG
GXX_ARGS += -g -DDEBUG
endif



##### PHONY RULES #####
.PHONY: default vulkantest hellotriangle all dirs clean
default: all

all: vulkantest hellotriangle

clean:
	-find $(OBJ) -name "*.o" -type f -delete
	-find $(BIN) -type f ! -name "*.*" -delete
	-find $(BIN) -name "*.out" -type f -delete



##### DIRECTORY RULES #####

$(BIN):
	mkdir -p $@
$(OBJ):
	mkdir -p $@
$(OBJ)/VulkanTest:
	mkdir -p $@
dirs: $(DIRS)



##### COMPILE RULES #####

# Compile rule for the main.cpp, which skips object rules for now
$(BIN)/vulkantest: $(VULKANTEST)/main.cpp | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^ $(GXX_LINK)
vulkantest: $(BIN)/vulkantest

# Compile rule for the main.cpp of the Hello Triangle tutorial, which skips object rules for now
$(BIN)/hellotriangle: $(HELLOTRIANGLE)/main.cpp | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^ $(GXX_LINK)
hellotriangle: $(BIN)/hellotriangle
