#include "MNISTLoader.h"
#include <fstream>
#include <stdexcept>
#include <cstdint>
using namespace std;

// Internal helper: Flips a 32-bit integer from Big-Endian to Little-Endian
uint32_t swapEndian(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

vector<Matrix> loadMNISTImages(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open MNIST image file: " + filename);
    }

    uint32_t magic = 0, numImages = 0, rows = 0, cols = 0;
    file.read(reinterpret_cast<char*>(&magic), 4);
    file.read(reinterpret_cast<char*>(&numImages), 4);
    file.read(reinterpret_cast<char*>(&rows), 4);
    file.read(reinterpret_cast<char*>(&cols), 4);

    numImages = swapEndian(numImages);
    rows = swapEndian(rows);
    cols = swapEndian(cols);

    if (numImages > 5000) {
        numImages = 5000;
    }
    int imageSize = rows * cols; 
    vector<Matrix> allImages;
    allImages.reserve(numImages);

    for (uint32_t i = 0; i < numImages; i++) {
        Matrix img(imageSize, 1);
        for (int p = 0; p < imageSize; p++) {
            unsigned char pixel = 0;
            file.read(reinterpret_cast<char*>(&pixel), 1);
            img.data[p][0] = static_cast<float>(pixel) / 255.0f;
        }
        allImages.push_back(img);
    }
    
    file.close();
    return allImages;
}

vector<int> loadMNISTLabels(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open MNIST label file: " + filename);
    }

    uint32_t magic = 0, numLabels = 0;
    file.read(reinterpret_cast<char*>(&magic), 4);
    file.read(reinterpret_cast<char*>(&numLabels), 4);

    if (numLabels > 5000) {
        numLabels = 5000;
    }

    numLabels = swapEndian(numLabels);

    vector<int> labels(numLabels);
    for (uint32_t i = 0; i < numLabels; i++) {
        unsigned char label = 0;
        file.read(reinterpret_cast<char*>(&label), 1);
        labels[i] = static_cast<int>(label);
    }

    file.close();
    return labels;
}
