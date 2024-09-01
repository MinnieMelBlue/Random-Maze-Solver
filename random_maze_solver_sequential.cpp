#include "maze.h"
#include "particle.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

#define DEBUG_MODE
#ifdef DEBUG_MODE
#define DEBUG_MSG(msg) std::cout << "DEBUG: " << msg << std::endl
#else
#define DEBUG_MSG(msg)
#endif

int main() {
    // Maze files and particle counts
    //std::vector<std::string> mazeFiles = {"maze_50.txt", "maze_100.txt"}; // datasets
    std::vector<std::string> mazeFiles = {"maze_50.txt"}; // datasets
    std::vector<int> particleCounts = {50,100};  // parameter

    for (const auto& mazeFilename : mazeFiles) {
        Maze maze;
        int startX = 1, startY = 1;

        // Load the maze
        if (!maze.loadFromFile(mazeFilename)) {
            std::cerr << "Failed to load maze from file: " << mazeFilename << std::endl;
            continue; // Skip to the next maze file
        }

        // Iterate over different numbers of particles
        for (int numParticles : particleCounts) {
            DEBUG_MSG("Simulating " << numParticles << " particles...");

            std::vector<Particle> particles(numParticles, Particle(startX, startY));
            std::vector<std::vector<std::pair<int, int>>> particlePaths(numParticles);
            std::vector<std::pair<int, int>> exitPath;

            bool foundExit = false;
            int particleThatFoundExit = -1;

            // Start timing
            auto startTime = std::chrono::high_resolution_clock::now();

            // Simulate all particles
            while (!foundExit) {
                for (int i = 0; i < numParticles; ++i) {
                    auto& particle = particles[i];

                    particle.move(maze.getData());
                    particlePaths[i] = particle.getVisitedCells();

                    if (maze.getData()[particle.getY()][particle.getX()] == Maze::EXIT) {
                        DEBUG_MSG("Particle " << i << " found the exit at (" << particle.getX() << ", " << particle.getY() << ")");
                        foundExit = true;
                        particleThatFoundExit = i;
                        break; // particle has found exit
                    }
                }
            }

            // End timing
            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = endTime - startTime;

            if (foundExit) {
                exitPath = particles[particleThatFoundExit].getVisitedCells();
            }
            std::cout << "Simulation finished for " << numParticles << " particles." << std::endl;
            std::cout << "Time taken: " << std::fixed << std::setprecision(4) << elapsed.count() << " seconds" << std::endl;

            // Save the maze with all particle paths
            std::string imageFilename = "../output/sequential_"+mazeFilename.substr(0, mazeFilename.find_last_of('.')) + "_after_particles_" + std::to_string(numParticles) + ".png";
            maze.saveAsImage(imageFilename, particlePaths, exitPath, true);
        }
    }

    return 0;
}
