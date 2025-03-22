########## Directories ##########
# Name of the Executables
PROJECT_EXE = cppchess

# Name of the Directories
DIR_SRC = src
DIR_HEADERS = include
DIR_OBJS = obj
DIR_BUILD = build/debug

########## Compiler ##########
CC = g++

# Compiler flags (strictly for the compiler)
CFLAGS = -std=c++11 -Wall -O0 -g

# Include paths
INCLUDE_FLAGS = -I$(DIR_HEADERS) -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -D_THREAD_SAFE -I/opt/homebrew/opt/libpng/include/libpng16 -I/opt/homebrew/opt/jpeg-xl/include -I/opt/homebrew/Cellar/highway/1.2.0/include -DHWY_SHARED_DEFINE -I/opt/homebrew/Cellar/brotli/1.1.0/include -I/opt/homebrew/opt/little-cms2/include -I/opt/homebrew/Cellar/libtiff/4.7.0/include -I/opt/homebrew/opt/zstd/include -I/opt/homebrew/Cellar/xz/5.6.4/include -I/opt/homebrew/opt/jpeg-turbo/include -I/opt/homebrew/Cellar/libavif/1.2.0/include -DAVIF_DLL -I/opt/homebrew/Cellar/aom/3.12.0/include -I/opt/homebrew/Cellar/libvmaf/3.0.0/include -I/opt/homebrew/Cellar/libvmaf/3.0.0/include/libvmaf -I/opt/homebrew/opt/webp/include -I/opt/homebrew/opt/webp/include/webp

# Linker flags and libraries
LINKER_FLAGS = -L/opt/homebrew/lib -lSDL2 -L/opt/homebrew/Cellar/sdl2_image/2.8.8/lib -lSDL2_image 
# LIBS = -lm -lSDL2 -lSDL2_image

########## List with Files ##########
SRC_FILES = $(wildcard $(DIR_SRC)/*.cpp)
OBJ_FILES = $(patsubst $(DIR_SRC)/%.cpp, $(DIR_OBJS)/%.o, $(SRC_FILES))

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