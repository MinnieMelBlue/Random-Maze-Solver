cmake_minimum_required(VERSION 3.26)
project(Random_Maze_Solver)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

find_package(OpenMP REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

# Add directories for OpenMP headers
include_directories("/opt/homebrew/Cellar/libomp/18.1.8/include")


# Add executables
add_executable(maze_generation
        maze_generation.cpp
        maze.cpp
)

add_executable(random_maze_solver_sequential
        maze.cpp
        particle.cpp
        random_maze_solver_sequential.cpp
)
add_executable(random_maze_solver_parallel
        random_maze_solver_parallel.cpp
        maze.cpp
        particle.cpp
)

# Link SFML libraries
target_link_libraries(random_maze_solver_sequential sfml-graphics)
target_link_libraries(random_maze_solver_parallel sfml-graphics ${OpenMP_CXX_LIBRARIES})
target_link_libraries(maze_generation sfml-graphics)

# Apply OpenMP flags only for parallel version
if(OpenMP_CXX_FOUND)
    target_compile_options(random_maze_solver_parallel PRIVATE ${OpenMP_CXX_FLAGS})
endif()
