cmake_minimum_required (VERSION 3.0)

project ("Project_SDL1")

set (CMAKE_CXX_STANDARD 17)

add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
add_link_options(-fsanitize=address -fno-omit-frame-pointer)

# Schließen Sie Unterprojekte ein.
add_subdirectory ("Project_SDL_Part1_base")
add_subdirectory ("Project_SDL_Part2_base")
