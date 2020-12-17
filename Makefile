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
HELLOTRIANGLE_SHADERS=$(HELLOTRIANGLE)/shaders
HELLOTRIANGLE_SHADERS_BIN=$(HELLOTRIANGLE_SHADERS)/bin



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
	-find $(HELLOTRIANGLE_SHADERS_BIN) -name "*.spv" -type f -delete



##### DIRECTORY RULES #####

$(BIN):
	mkdir -p $@
$(OBJ):
	mkdir -p $@
$(OBJ)/VulkanTest:
	mkdir -p $@
$(OBJ)/HelloTriangle:
	mkdir -p $@
$(HELLOTRIANGLE_SHADERS_BIN):
	mkdir -p $@
dirs: $(BIN) $(OBJ) $(OBJ)/VulkanTest $(OBJ)/HelloTriangle $(HELLOTRIANGLE_SHADERS_BIN)



##### COMPILE RULES #####

# Compile rule for the main.cpp, which skips object rules for now
$(BIN)/vulkantest: $(VULKANTEST)/main.cpp | dirs
	$(GXX) $(GXX_ARGS) -o $@ $^ $(GXX_LINK)
vulkantest: $(BIN)/vulkantest

# General compile rule for all shaders in the HelloTriangle subproject
$(HELLOTRIANGLE_SHADERS_BIN)/%.spv: $(HELLOTRIANGLE_SHADERS)/shader.% | dirs
	glslc -o $@ $<

# Compile rule for the main.cpp of the Hello Triangle tutorial, which skips object rules for now
$(BIN)/hellotriangle: $(HELLOTRIANGLE)/main.cpp $(HELLOTRIANGLE_SHADERS_BIN)/vert.spv $(HELLOTRIANGLE_SHADERS_BIN)/frag.spv | dirs
	$(GXX) $(GXX_ARGS) -o $@ $< $(GXX_LINK)
hellotriangle: $(BIN)/hellotriangle
