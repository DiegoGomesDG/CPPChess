########## Directories ##########
# Name of the Executables
PROJECT_EXE = cppchess

# Name of the Directories
DIR_SRC = src
DIR_HEADERS = include
DIR_OBJS = obj
DIR_BUILD = build
IMGUI_DIR = lib/imgui

########## Compiler ##########
CC = g++

# Compiler flags (strictly for the compiler)
CFLAGS = -std=c++11 -Wall -O0 -g


# Include paths
# pkg-config --cflags --libs sdl2 SDL2_mixer SDL2_image SDL2_ttf
INCLUDE_FLAGS = -I$(DIR_HEADERS) -I/opt/homebrew/include/SDL2 -I/opt/homebrew/Cellar/libxmp/4.6.2/include -I/opt/homebrew/Cellar/fluid-synth/2.4.3/include -I/opt/homebrew/Cellar/glib/2.84.1/include -I/opt/homebrew/Cellar/libsndfile/1.2.2_1/include -I/opt/homebrew/Cellar/libvorbis/1.3.7/include -I/opt/homebrew/Cellar/portaudio/19.7.0/include -I/opt/homebrew/Cellar/readline/8.2.13/include -D_DARWIN_C_SOURCE -I/opt/homebrew/Cellar/game-music-emu/0.6.4/include -I/opt/homebrew/Cellar/flac/1.5.0/include -I/opt/homebrew/Cellar/mpg123/1.32.10/include -I/opt/homebrew/Cellar/opusfile/0.12_1/include/opus -I/opt/homebrew/Cellar/libogg/1.3.5/include -I/opt/homebrew/Cellar/opus/1.5.2/include/opus -I/opt/homebrew/Cellar/wavpack/5.8.1/include -I/opt/homebrew/opt/jpeg-xl/include -I/opt/homebrew/Cellar/highway/1.2.0/include -DHWY_SHARED_DEFINE -I/opt/homebrew/Cellar/brotli/1.1.0/include -I/opt/homebrew/opt/little-cms2/include -I/opt/homebrew/Cellar/libtiff/4.7.0/include -I/opt/homebrew/opt/zstd/include -I/opt/homebrew/Cellar/xz/5.8.1/include -I/opt/homebrew/opt/jpeg-turbo/include -I/opt/homebrew/Cellar/libavif/1.2.0/include -DAVIF_DLL -I/opt/homebrew/Cellar/aom/3.12.1/include -I/opt/homebrew/Cellar/libvmaf/3.0.0/include -I/opt/homebrew/Cellar/libvmaf/3.0.0/include/libvmaf -I/opt/homebrew/opt/webp/include -I/opt/homebrew/opt/webp/include/webp -I/opt/homebrew/include -D_THREAD_SAFE -I/opt/homebrew/Cellar/harfbuzz/11.1.0/include/harfbuzz -I/opt/homebrew/Cellar/glib/2.84.1/include/glib-2.0 -I/opt/homebrew/Cellar/glib/2.84.1/lib/glib-2.0/include -I/opt/homebrew/opt/gettext/include -I/opt/homebrew/Cellar/pcre2/10.45/include -I/opt/homebrew/Cellar/graphite2/1.3.14/include -I/opt/homebrew/opt/freetype/include/freetype2 -I/opt/homebrew/opt/libpng/include/libpng16 -L/opt/homebrew/Cellar/sdl2_mixer/2.8.1_1/lib -lSDL2_mixer -L/opt/homebrew/Cellar/sdl2_image/2.8.8/lib -lSDL2_image -L/opt/homebrew/Cellar/sdl2_ttf/2.24.0/lib -lSDL2_ttf -L/opt/homebrew/lib -lSDL2

# Linker flags and libraries
# pkg-config --libs sdl2 SDL2_mixer SDL2_image SDL2_ttf
LINKER_FLAGS = -L/opt/homebrew/Cellar/sdl2_mixer/2.8.1_1/lib -lSDL2_mixer -L/opt/homebrew/Cellar/sdl2_image/2.8.8/lib -lSDL2_image -L/opt/homebrew/Cellar/sdl2_ttf/2.24.0/lib -lSDL2_ttf -L/opt/homebrew/lib -lSDL2
LINKER_FLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends 
# LIBS = -lm -lSDL2 -lSDL2_image

########## List with Files ##########
SRC_FILES = $(wildcard $(DIR_SRC)/*.cpp)
OBJ_FILES = $(patsubst $(DIR_SRC)/%.cpp, $(DIR_OBJS)/%.o, $(SRC_FILES))

SRC_FILES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SRC_FILES += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp

########## Make ##########
# Default target: build the main executable
#all: $(DIR_BUILD)/$(PROJECT_EXE)
all:
	$(CC) $(CFLAGS) $(LINKER_FLAGS) $(INCLUDE_FLAGS) $(SRC_FILES) -o $(DIR_BUILD)/$(PROJECT_EXE)

# Rule to build the main executable
#$(DIR_BUILD)/$(PROJECT_EXE): $(OBJ_FILES)
#	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(OBJ_FILES) -o $@ $(LIBS)

# Rule to compile source files into object files
#$(DIR_OBJS)/%.o: $(DIR_SRC)/%.cpp
#	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Run the project
run: $(DIR_BUILD)/$(PROJECT_EXE)
	./$(DIR_BUILD)/$(PROJECT_EXE)

# Clean build artifacts


# Phony targets
.PHONY: all run

# Generate compile_commands.json for the clangd