#ifndef MNISTLOADER_H
#define MNISTLOADER_H

#include <vector>
#include <string>
#include "Matrix.h"

// High-level utility functions to load the MNIST dataset
std::vector<Matrix> loadMNISTImages(const std::string& filename);
std::vector<int> loadMNISTLabels(const std::string& filename);

#endif // MNISTLOADER_H