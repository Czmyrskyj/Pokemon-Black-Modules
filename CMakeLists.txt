cmake_minimum_required(VERSION 4.3.0)
set(Project_Name pokemon-black-modules)
project(${Project_Name} LANGUAGES CXX)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

find_program(MOLD_PATH mold)
if (MOLD_PATH)
    add_link_options("-fuse-ld=mold")
    message(STATUS "Using mold linker: ${MOLD_PATH}")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(raylib)

add_executable(${Project_Name} 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/battle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/assets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/trainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_title.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_main_menu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_wifi_connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_boy_or_girl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_whats_your_name.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_juniper_letter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/locale.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/formatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/writer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_choose_a_starter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pokedex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_starter_selected.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_which_season_and_region.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/battle_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pokemon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/music-offset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state_battle_vs.cpp
)

target_precompile_headers(${Project_Name} PRIVATE
    <raylib.h>
    <vector>
    <string>
)

target_compile_options(${Project_Name} PRIVATE
	-Wall
	-Wextra
	-Wpedantic

	-Wshadow
	-Wnon-virtual-dtor
	-Wold-style-cast
	-Wcast-align
	-Wnull-dereference
	-Wdouble-promotion
	-Wformat=2
	-Wmisleading-indentation
	-Wduplicated-cond
	-Wduplicated-branches
	-Wlogical-op

	-Wunused
	-Werror
)

target_include_directories(${Project_Name} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${Project_Name} PRIVATE
	raylib
)
