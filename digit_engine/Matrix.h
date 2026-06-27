#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

class Matrix {
public:
    int rows;
    int cols;
    std::vector<std::vector<float>> data;

    // Constructors
    Matrix(int r, int c);
    Matrix(int r, int c, float initialValue);

    void save(std::ofstream& outFile) const; //for the websie
    void load(std::ifstream& inFile);

    //matrix math
    static Matrix multiply(const Matrix& a, const Matrix& b); //static so we don't have to type a.multiply()
    void add(const Matrix& b);                                //element wise addition

    // neural network operation 
    void applyReLU(); //for forward propagation
    Matrix deriveReLU() const; //for backward propagation

    //for backward propagation 
    static Matrix transpose(const Matrix& m);
    void multiplyElementWise(const Matrix& b);
    
    // utility functions
    void randomize(); //to fill matrix with random weights
    void print() const; // to print the matrix for debuggin
};

#endif 