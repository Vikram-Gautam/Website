#include <bits/stdc++.h>
#include "Matrix.h"
#include <fstream>

using namespace std;

// Constructor 
Matrix::Matrix(int r, int c) : rows(r), cols(c) {
    data.resize(rows, vector<float>(cols, 0.0f)); //single precision pointer 4byte to save memory
}

// Constructor to initialize with base value
Matrix::Matrix(int r, int c, float initialValue) : rows(r), cols(c) {
    data.resize(rows, vector<float>(cols, initialValue));
}

//fill the matrix with random values between -0.5 and 0.5, we are using Xavier/He standard practice. if the numbers were to far part 
void Matrix::randomize() { // they can slow up to NaN. it should be 0.8 but 0.5 seems fine as well

    random_device rd; // random seed, I think it uses the cpu temperature 
    mt19937 gen(rd()); // Mersenne Twister algo, I don't know what it does, but I can't use rand as it is too predictable

    uniform_real_distribution<float> dist(-0.5f, 0.5f); // dist is a uniform dist between +-0.5

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i][j] = dist(gen); // converts the random number from gen to a range between +-0.5
        }
    }
}


Matrix Matrix::multiply(const Matrix& a, const Matrix& b) {
    if (a.cols != b.rows) {
        throw invalid_argument("Dimension mismatch"); // will halt the program and throw a error message
    }

    Matrix result(a.rows, b.cols);
    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < b.cols; j++) {
            float sum = 0.0f;
            for (int k = 0; k < a.cols; k++) {
                sum += a.data[i][k] * b.data[k][j];
            }
            result.data[i][j] = sum;
        }
    }
    return result;
}


void Matrix::add(const Matrix& b) { //element wise additoin a.add(b)
    if (rows != b.rows || cols != b.cols) {
        throw invalid_argument("Dimension mismatch");
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i][j] += b.data[i][j];
        }
    }
}

// apply ReLU 
void Matrix::applyReLU() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i][j] = max(0.0f, data[i][j]);
        }
    }
}

Matrix Matrix::transpose(const Matrix& m) { //transpose
    Matrix result(m.cols, m.rows);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            result.data[j][i] = m.data[i][j];
        }
    }
    return result;
}

void Matrix::multiplyElementWise(const Matrix& b) { 
    if (rows != b.rows || cols != b.cols) {
        throw invalid_argument("Mismatch of dimension");
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i][j] *= b.data[i][j];
        }
    }
}
// for backward propagation
Matrix Matrix::deriveReLU() const {
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Derivative is 1 if input > 0, otherwise 0
            result.data[i][j] = (data[i][j] > 0.0f) ? 1.0f : 0.0f;
        }
    }
    return result;
}

//for debug
void Matrix::print() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << data[i][j] << " ";
        }
        cout << "\n";
    }
}


void Matrix::save(std::ofstream& outFile) const {
    // Write rows and columns first so we know the structure on load
    outFile << rows << " " << cols << "\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            outFile << data[i][j] << " ";
        }
        outFile << "\n";
    }
}

void Matrix::load(std::ifstream& inFile) {
    int r, c;
    if (inFile >> r >> c) {
        // Ensure our internal matrix shape matches what's stored in the file
        this->rows = r;
        this->cols = c;
        this->data.assign(r, std::vector<float>(c, 0.0));
        
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                inFile >> data[i][j];
            }
        }
    }
}