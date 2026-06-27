#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "Matrix.h"
#include <vector>

class NeuralNetwork {
private:
    // Model Layers Dimensions
    int inputNodes;
    int hiddenNodes;
    int outputNodes;

    // Weight matrix
    Matrix weightsIH; // input to hidden
    Matrix weightsHO; // hidden to output

    // Bias matrix
    Matrix biasH;//for hidden layer
    Matrix biasO;//for output layer

    
    Matrix softmax(const Matrix& m) const; // to have all values between 0 and 1

public:
    //Constructor
    NeuralNetwork(int inputNum, int hiddenNum, int outputNum);

    // actual training method for backward propagation
    void train(const Matrix& inputVector, int targetLabel, float learningRate);
    //forward propagation
    Matrix forward(const Matrix& inputVector);

    void saveWeights(const std::string& filename) const;
   bool loadWeights(const std::string& filename);
};


#endif 