cmake_minimum_required(VERSION 4.3.0)
set(Project_Name pokemon-black-modules)
project(${Project_Name} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
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
)
add_compile_options(
	-Wall
	-Wextra
	-Wpedantic
	-Wold-style-cast
	-Wcast-align
	-Wcast-qual
	-Wconversion
	-Wsign-conversion
	-Wuseless-cast
	-Wdouble-promotion
	-Wunused
	-Wunused-parameter
	-Wunused-result
	-Wshadow
	-Wshadow-all
	-Wfloat-equal
	-Woverloaded-virtual
	-Wnon-virtual-dtor
	-Wctor-dtor-privacy
	-Wformat=2
	-Wformat-security
	-Wnull-dereference
	-Wmisleading-indentation
	-Wduplicated-cond
	-Wduplicated-branches
	-Wlogical-op
	-Wredundant-decls
	-Wzero-as-null-pointer-constant
	-Werror
)
target_include_directories(${Project_Name} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${Project_Name}
	raylib
)
