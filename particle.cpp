#include "particle.h"
#include "maze.h"
#include <cstdlib>  // For rand()
#include <iostream>

Particle::Particle(int startX, int startY) : x(startX), y(startY) {
    visitedCells.push_back({x, y}); // Add the starting position to the visited cells
}

void Particle::move(const std::vector<std::vector<int>>& maze) {
    const int directions[4][2] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
    int dir = rand() % 4;
    int nx = x + directions[dir][0];
    int ny = y + directions[dir][1];

  //  std::cout << "Trying to move particle from (" << x << ", " << y << ") to (" << nx << ", " << ny << ")\n";

    // Check if the new position is within bounds and not a wall
    if (nx >= 0 && ny >= 0 && nx < maze[0].size() && ny < maze.size() && maze[ny][nx] != Maze::WALL) {
        x = nx;
        y = ny;
        visitedCells.push_back({x, y}); // Add the new position to the visited cells
    }
}

int Particle::getX() const {
    return x;
}

int Particle::getY() const {
    return y;
}

const std::vector<std::pair<int, int>>& Particle::getVisitedCells() const {
    return visitedCells;
}
