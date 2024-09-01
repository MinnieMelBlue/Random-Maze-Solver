#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <utility> // for std::pair

class Particle {
public:
    Particle(int startX, int startY);

    void move(const std::vector<std::vector<int>>& maze);

    int getX() const;
    int getY() const;
    const std::vector<std::pair<int, int>>& getVisitedCells() const;

private:
    int x, y; // Current position of the particle
    std::vector<std::pair<int, int>> visitedCells; // Stores all visited cells by the particle
};

#endif // PARTICLE_H
