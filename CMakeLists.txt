cmake_minimum_required(VERSION 3.15)
project(cppchess)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Set output directories
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Find Homebrew installation path
execute_process(
  COMMAND brew --prefix sdl2
  OUTPUT_VARIABLE HOMEBREW_SDL2_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND brew --prefix sdl2_image
  OUTPUT_VARIABLE HOMEBREW_SDL2_IMAGE_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND brew --prefix sdl2_mixer
  OUTPUT_VARIABLE HOMEBREW_SDL2_MIXER_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND brew --prefix sdl2_ttf
  OUTPUT_VARIABLE HOMEBREW_SDL2_TTF_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Manually set SDL2 paths for Homebrew
set(SDL2_INCLUDE_DIR "${HOMEBREW_SDL2_PREFIX}/include/SDL2")
set(SDL2_LIBRARY "${HOMEBREW_SDL2_PREFIX}/lib/libSDL2.dylib")

set(SDL2_IMAGE_INCLUDE_DIR "${HOMEBREW_SDL2_IMAGE_PREFIX}/include/SDL2")
set(SDL2_IMAGE_LIBRARY "${HOMEBREW_SDL2_IMAGE_PREFIX}/lib/libSDL2_image.dylib")

set(SDL2_MIXER_INCLUDE_DIR "${HOMEBREW_SDL2_MIXER_PREFIX}/include/SDL2")
set(SDL2_MIXER_LIBRARY "${HOMEBREW_SDL2_MIXER_PREFIX}/lib/libSDL2_mixer.dylib")

set(SDL2_TTF_INCLUDE_DIR "${HOMEBREW_SDL2_TTF_PREFIX}/include/SDL2")
set(SDL2_TTF_LIBRARY "${HOMEBREW_SDL2_TTF_PREFIX}/lib/libSDL2_ttf.dylib")

# ImGui setup (using your submodule)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)

# Source files
set(SRC_FILES
    src/main.cpp
    src/Bishop.cpp
    src/Board.cpp
    src/ChessGUI.cpp
    src/Game.cpp
    src/Graphics.cpp
    src/King.cpp
    src/Knight.cpp
    src/Pawn.cpp
    src/Piece.cpp
    src/Queen.cpp
    src/Rook.cpp
    src/Texture.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
    ${SDL2_TTF_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    ${SDL2_TTF_LIBRARY}
)

# macOS-specific settings
if(APPLE)
    # Remove bundle settings if not needed
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE FALSE
    )

    find_library(COCOA_LIBRARY Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    target_link_libraries(${PROJECT_NAME}
        ${COCOA_LIBRARY}
        ${METAL_LIBRARY}
        ${QUARTZCORE_LIBRARY}
    )
endif()

# Custom run target
add_custom_target(run
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
)