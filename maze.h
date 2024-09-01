#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <string>

class Maze {
public:
    Maze();

    // Methods to initialize, load, and save the maze
    void initialize(int width, int height, int startX, int startY, int exitX, int exitY);
    bool loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void saveAsImage(const std::string& filename,
                     const std::vector<std::vector<std::pair<int, int>>>& particlePaths,
                     const std::vector<std::pair<int, int>>& exitPath,
                     bool drawAdditionalElements) const;

    // Getters for maze dimensions and size
    int getWidth() const;
    int getHeight() const;
    int getSize() const;  // New method to get the size of the maze

    // Getter for the maze data
    const std::vector<std::vector<int>>& getData() const;

    // Enum for cell types, made public for access in other classes
    enum CellType { WALL = 0, PATH = 1, START = 2, EXIT = 3 };

private:
    // Method to generate the maze using DFS
    void generateMaze(int startX, int startY);

    // Helper method to validate cell coordinates
    bool isValid(int x, int y) const;

    // Maze attributes
    int width;
    int height;
    int startX;
    int startY;
    int exitX;
    int exitY;
    std::vector<std::vector<int>> data;  // 2D grid representing the maze
};

#endif // MAZE_H
