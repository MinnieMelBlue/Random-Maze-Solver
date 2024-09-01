#include "maze.h"
#include "particle.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <chrono>
#include <iomanip>
#include <omp.h>  // Include OpenMP header
#include <fstream> // Include fstream for CSV file operations

namespace fs = std::filesystem;

#define DEBUG_MODE
#ifdef DEBUG_MODE
#define DEBUG_MSG(msg) std::cout << "DEBUG: " << msg << std::endl
#else
#define DEBUG_MSG(msg)
#endif

int main() {
    std::vector<std::string> mazeFiles = {"maze_50.txt"};
    std::vector<int> particleCounts = {50, 100};
    std::vector<int> threadCounts = {2,4,6,8,10,12};  // Array of thread counts

    // Open CSV file for writing results
    std::ofstream csvFile("../output/simulation_times.csv");

    // Write headers to CSV file
    csvFile << "Dataset,Particles,Threads,Time (seconds)\n";

    for (const auto& mazeFilename : mazeFiles) {
        Maze maze;

        // Load the maze
        if (!maze.loadFromFile(mazeFilename)) {
            std::cerr << "Failed to load maze from file: " << mazeFilename << std::endl;
            continue;
        }

        for (int numParticles : particleCounts) {
            for (int numThreads : threadCounts) {  // Loop through different thread counts
                DEBUG_MSG("Simulating " << numParticles << " particles with " << numThreads << " threads...");

                std::vector<std::vector<std::pair<int, int>>> particlePaths(numParticles);
                std::vector<std::pair<int, int>> exitPath;
                bool foundExit = false;
                int particleThatFoundExit = -1;

                // Set the number of threads for this simulation
                omp_set_num_threads(numThreads);

                // Start timing
                auto startTime = std::chrono::high_resolution_clock::now();

#pragma omp parallel
                {
                    // Each thread will simulate particles
#pragma omp single
                    {
                        DEBUG_MSG("Starting parallel simulation with " << omp_get_num_threads() << " threads.");
                    }

#pragma omp for schedule(dynamic)
                    for (int i = 0; i < numParticles; ++i) {
                        Particle particle(1, 1);
                        std::vector<std::pair<int, int>> path;

                        while (!foundExit) {
                            particle.move(maze.getData());
                            path = particle.getVisitedCells();

                            // Check for exit and update shared variables if needed
                            if (maze.getData()[particle.getY()][particle.getX()] == Maze::EXIT) {
#pragma omp critical
                                {
                                    if (!foundExit) {
                                        foundExit = true;
                                        particleThatFoundExit = i;
                                        exitPath = path;
                                    }
                                }
                                break; // Exit the while loop
                            }
                        }
                        particlePaths[i] = path; // Save the path for this particle
                    }
                }

                // End timing
                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = endTime - startTime;

                std::cout << "Simulation finished for " << numParticles << " particles with " << numThreads << " threads." << std::endl;
                std::cout << "Time taken: " << std::fixed << std::setprecision(4) << elapsed.count() << " seconds" << std::endl;

                // Save the maze with all particle paths
                std::string imageFilename = "../output/parallel_" + mazeFilename.substr(0, mazeFilename.find_last_of('.')) +
                                            "_after_particles_" + std::to_string(numParticles) +
                                            "_threads_" + std::to_string(numThreads) + ".png";
                maze.saveAsImage(imageFilename, particlePaths, exitPath, true);

                // Write results to CSV
                csvFile << mazeFilename.substr(mazeFilename.find_last_of('/') + 1) << ","
                        << numParticles << ","
                        << numThreads << ","
                        << std::fixed << std::setprecision(4) << elapsed.count() << "\n";
            }
        }
    }

    // Close CSV file
    csvFile.close();

    return 0;
}
