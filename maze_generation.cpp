#include "maze.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG_MSG(msg) std::cout << "DEBUG: " << msg << std::endl
#else
#define DEBUG_MSG(msg)
#endif

void generateMaze(const std::string& filename, int size) {
    Maze maze;
    const int startX = 1, startY = 1;
    int exitX = size - 2, exitY = size - 2;
    std::string extensions[2] = {".txt", ".png"};

    DEBUG_MSG("Initializing a new maze of size " << size << "x" << size);
    maze.initialize(size, size, startX, startY, exitX, exitY);
    maze.saveToFile(filename + extensions[0]);
    if (size < 100) {
        maze.saveAsImage(filename + ".png", {}, {}, false);
        DEBUG_MSG("Maze image saved as " << filename + extensions[1]);
    }
}

int main() {
    // sizes
    //    std::vector<int> sizes = {50, 100,1000};
    std::vector<int> sizes = {50,100};
    for (int size : sizes) {
        std::string filename = "../input/maze_" + std::to_string(size);
        generateMaze(filename, size);
    }
    return 0;
}
