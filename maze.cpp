#include "maze.h"
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <stack>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG_MSG(msg) std::cout << "DEBUG: " << msg << std::endl
#else
#define DEBUG_MSG(msg)
#endif

Maze::Maze() : width(0), height(0), startX(0), startY(0), exitX(0), exitY(0) { }

// Helper function to check if a cell is within bounds and is a wall
bool Maze::isValid(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height && data[y][x] == WALL;
}

// Generate the maze using Depth-First Search (DFS)
void Maze::generateMaze(int startX, int startY) {
    const int directions[4][2] = { {0, 2}, {0, -2}, {2, 0}, {-2, 0} };

    std::stack<std::pair<int, int>> stack;
    stack.push({startX, startY});
    data[startY][startX] = PATH;

    std::mt19937 rng(12345); // Fixed seed for reproducibility

    while (!stack.empty()) {
        auto [x, y] = stack.top();
        stack.pop();

        std::vector<std::pair<int, int>> neighbors;
        for (const auto& dir : directions) {
            int nx = x + dir[0];
            int ny = y + dir[1];
            if (isValid(nx, ny)) {
                int wx = x + dir[0] / 2;
                int wy = y + dir[1] / 2;
                if (data[wy][wx] == WALL) {
                    neighbors.push_back({nx, ny});
                }
            }
        }

        if (!neighbors.empty()) {
            stack.push({x, y});
            std::shuffle(neighbors.begin(), neighbors.end(), std::mt19937{std::random_device{}()}); // Shuffle to add randomness
            auto [nx, ny] = neighbors.front();
            int wx = x + (nx - x) / 2;
            int wy = y + (ny - y) / 2;
            data[ny][nx] = PATH;
            data[wy][wx] = PATH;
            stack.push({nx, ny});
        }
    }

    // Ensure a direct path from start to exit
    if (startX != exitX || startY != exitY) {
        std::vector<std::pair<int, int>> path;
        int x = startX, y = startY;
        while (x != exitX || y != exitY) {
            path.push_back({x, y});
            if (x < exitX) x++;
            else if (x > exitX) x--;
            if (y < exitY) y++;
            else if (y > exitY) y--;
            if (data[y][x] != PATH) data[y][x] = PATH;
        }
        path.push_back({x, y});
    }

    DEBUG_MSG("Maze generation completed");
}

// Load the maze from a file
bool Maze::loadFromFile(const std::string& filename) {
    std::string fullPath = "../input/" + filename;
    std::ifstream file(fullPath);
    if (!file) {
        std::cerr << "Unable to open file for reading: " << fullPath << std::endl;
        return false;
    }

    file >> height >> width;
    if (height <= 0 || width <= 0) {
        std::cerr << "Invalid dimensions in maze file." << std::endl;
        return false;
    }

    data.resize(height, std::vector<int>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!(file >> data[y][x])) {
                std::cerr << "Error reading maze data from file." << std::endl;
                return false;
            }
        }
    }

    DEBUG_MSG("Maze loaded successfully from file: " << fullPath);
    return true;
}

// Initialize the maze with walls and set start and exit positions
void Maze::initialize(int width, int height, int startX, int startY, int exitX, int exitY) {
    if (width < 3 || height < 3) {
        std::cerr << "Width and height must be at least 3." << std::endl;
        return;
    }

    this->width = width;
    this->height = height;
    this->startX = startX;
    this->startY = startY;
    this->exitX = exitX;
    this->exitY = exitY;

    data = std::vector<std::vector<int>>(height, std::vector<int>(width, WALL));

    std::srand(static_cast<unsigned>(std::time(0))); // Seed for random number generation
    generateMaze(startX, startY);

    data[startY][startX] = START;
    data[exitY][exitX] = EXIT;

    DEBUG_MSG("Maze initialized. Start: (" << startX << ", " << startY << "), Exit: (" << exitX << ", " << exitY << ")");
}

// Save the maze to a file
void Maze::saveToFile(const std::string& filename) const {
    std::string fullPath = "../input/" + filename;
    std::ofstream file(fullPath);
    if (!file) {
        std::cerr << "Unable to open file for writing: " << filename << std::endl;
        return;
    }

    file << height << " " << width << "\n";
    for (const auto& row : data) {
        for (size_t col = 0; col < row.size(); ++col) {
            file << row[col];
            if (col < row.size() - 1) {
                file << ' ';
            }
        }
        file << "\n";
    }

    DEBUG_MSG("Maze saved to file: " << filename);
}

// Save the maze as an image
void Maze::saveAsImage(const std::string& filename,
                       const std::vector<std::vector<std::pair<int, int>>>& particlePaths,
                       const std::vector<std::pair<int, int>>& exitPath,
                       bool drawAdditionalElements) const {
    const int cellSize = 20;
    const int dotRadius = 2; // Radius of the dots

    // Create an image with the size of the maze
    sf::Image mazeImage;
    mazeImage.create(width * cellSize, height * cellSize, sf::Color::White);

    // Draw the maze
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            sf::Color color;
            switch (data[y][x]) {
                case WALL: color = sf::Color::Black; break;
                case PATH: color = sf::Color::White; break;
                case START: color = sf::Color::Green; break;
                case EXIT: color = sf::Color::Red; break;
                default: color = sf::Color::White; break;
            }
            for (int dy = 0; dy < cellSize; ++dy) {
                for (int dx = 0; dx < cellSize; ++dx) {
                    mazeImage.setPixel(x * cellSize + dx, y * cellSize + dy, color);
                }
            }
        }
    }

    // Draw additional elements if the flag is set to true
    if (drawAdditionalElements) {
        // Draw all paths with dots if paths exist
        if (!particlePaths.empty()) {
            sf::Color dotColor = sf::Color::Blue;

            for (const auto& path : particlePaths) {
                for (const auto& point : path) {
                    int px = point.first * cellSize + cellSize / 2;
                    int py = point.second * cellSize + cellSize / 2;

                    // Draw dot
                    for (int dy = -dotRadius; dy <= dotRadius; ++dy) {
                        for (int dx = -dotRadius; dx <= dotRadius; ++dx) {
                            if (dx * dx + dy * dy <= dotRadius * dotRadius) {
                                int x = px + dx;
                                int y = py + dy;
                                if (x >= 0 && x < width * cellSize && y >= 0 && y < height * cellSize) {
                                    mazeImage.setPixel(x, y, dotColor);
                                }
                            }
                        }
                    }
                }
            }

            DEBUG_MSG("Particle paths drawn on maze image.");
        }

        // Draw the exit path with a line if it exists
        if (!exitPath.empty()) {
            sf::Color lineColor = sf::Color::Red;

            for (size_t i = 1; i < exitPath.size(); ++i) {
                int x1 = exitPath[i - 1].first * cellSize + cellSize / 2;
                int y1 = exitPath[i - 1].second * cellSize + cellSize / 2;
                int x2 = exitPath[i].first * cellSize + cellSize / 2;
                int y2 = exitPath[i].second * cellSize + cellSize / 2;

                // Draw line between two points
                int dx = std::abs(x2 - x1);
                int dy = std::abs(y2 - y1);
                int sx = (x1 < x2) ? 1 : -1;
                int sy = (y1 < y2) ? 1 : -1;
                int err = dx - dy;

                while (true) {
                    if (x1 >= 0 && x1 < width * cellSize && y1 >= 0 && y1 < height * cellSize) {
                        mazeImage.setPixel(x1, y1, lineColor);
                    }
                    if (x1 == x2 && y1 == y2) break;
                    int e2 = 2 * err;
                    if (e2 > -dy) { err -= dy; x1 += sx; }
                    if (e2 < dx) { err += dx; y1 += sy; }
                }
            }

            DEBUG_MSG("Exit path drawn on maze image.");
        }
    }

    // Save the image to file
    if (mazeImage.saveToFile(filename)) {
        DEBUG_MSG("Maze image saved to: " << filename);
    } else {
        std::cerr << "Failed to save maze image to: " << filename << std::endl;
    }
}

// Getter for width
int Maze::getWidth() const {
    return width;
}

// Getter for height
int Maze::getHeight() const {
    return height;
}

// Getter for maze data
const std::vector<std::vector<int>>& Maze::getData() const {
    return data;
}

int Maze::getSize() const {
    return width * height;
}
