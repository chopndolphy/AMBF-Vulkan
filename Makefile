# Directories
SRC = src
BUILD = build
INCLUDE = include
LIB = lib

ALL_INCLUDES = -I./$(LIB) -I./$(INCLUDE) -I./$(FASTGLTF)/include/ -I./$(FASTGLTF)/deps/simdjson/ -I./$(FMT)/include/ -I./$(GLM) -I./$(IMGUI) -I./$(SDL)/include/ -I./$(STB_IMAGE) -I./$(VKBOOTSTRAP)/src -I./$(VMA) -I./$(VOLK) 
FASTGLTF = $(LIB)/fastgltf
FMT = $(LIB)/fmt
GLM = $(LIB)/glm
IMGUI = $(LIB)/imgui
SDL = $(LIB)/SDL
STB_IMAGE = $(LIB)/stb_image
VKBOOTSTRAP = $(LIB)/vkbootstrap
VMA = $(LIB)/vma
VOLK = $(LIB)/volk

IMGUI_SOURCES := $(IMGUI)/imgui.cpp $(IMGUI)/imgui_demo.cpp $(IMGUI)/imgui_draw.cpp $(IMGUI)/imgui_tables.cpp $(IMGUI)/imgui_widgets.cpp $(IMGUI)/backends/imgui_impl_sdl2.cpp $(IMGUI)/backends/imgui_impl_vulkan.cpp


# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -O2 -g $(ALL_INCLUDES)
# CFLAGS = -Wall -ggdb -O3 $(INCLUDES)
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -pedantic -g -fsanitize=address $(ALL_INCLUDES)
# CXXFLAGS = -Wall -ggdb -O3 $(INCLUDES)
LDFLAGS = -lasan -lvulkan -lXxf86vm -lX11 -lpthread -lXrandr -lXi -ldl -lSDL2

# SHARED OBJECTS AND TARGETS  (Targets are executables)

# Shared objects by multiple executables
CPP_FILES := camera.cpp vk_descriptors.cpp vk_engine.cpp vk_images.cpp vk_initializers.cpp vk_loader.cpp vk_pipelines.cpp 
OBJECTS := $(CPP_FILES:.cpp=.o) imgui_impl_sdl2.o imgui_impl_vulkan.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui.o volk.o
OBJECTS := $(addprefix $(BUILD)/, $(OBJECTS))

# Targets
CPP_EXEC := ambf_engine.cpp
TARGETS_OBJ := $(CPP_EXEC:%.cpp=$(BUILD)/%.o)
TARGETS := $(TARGETS_OBJ:%.o=%)

# Compiled Libraries
BUILTLIBS := -L ./$(FASTGLTF)/build/ -lfastgltf -lfastgltf_simdjson -L ./$(FMT)/build/ -lfmt -L ./$(VKBOOTSTRAP)/build/ -lvk-bootstrap
# BUILTLIBS := -L ./$(FASTGLTF)/build/ -lfastgltf -lfastgltf_simdjson -L ./$(FMT)/build/ -lfmt -L ./$(SDL)/build/ -lSDL2main -lSDL2-2.0 -lSDL2 -Wl,-rpath,’$$ORIGIN’

# RECIPES
all: $(TARGETS)

# executables depend on shared objects
$(TARGETS): $(OBJECTS)

# Link
# Secondary expansions allow to use the automatic variable $@ in the prerequisites list.
# https://www.gnu.org/software/make/manual/html_node/Secondary-Expansion.html
.SECONDEXPANSION:
$(TARGETS): $(OBJECTS) $$@.o 
	$(CXX) -o $@ $^ $(LDFLAGS) $(BUILTLIBS)

# Compile objects
# Order of recipes matter. Recipe 2 has to be before recipe 3 to take into account .h prerrequisites. 

# imgui:
$(BUILD)/%.o:$(IMGUI)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#volk:
$(BUILD)/volk.o: $(VOLK)/volk.c $(VOLK)/volk.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

# recipe 2: compile objects - cpp files with header files
$(BUILD)/%.o: $(SRC)/%.cpp $(INCLUDE)/%.h | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

# recipe 3: compile executables - cpp files without header files
$(BUILD)/%.o: $(SRC)/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@    

# PHONY
.PHONY: all clean run

clean:
	rm -rf $(BUILD)

run: $(TARGETS)
	./$(BUILD)/ambf_engine

$(BUILD):
	mkdir -p $(BUILD)