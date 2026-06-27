#include <bits/stdc++.h>
#include "NeuralNetwork.h"
#include <fstream>
using namespace std;

//Constrctor, initializes with random weights 
NeuralNetwork::NeuralNetwork(int inputNum, int hiddenNum, int outputNum) 
    : inputNodes(inputNum), hiddenNodes(hiddenNum), outputNodes(outputNum),// don't forget the :
      weightsIH(hiddenNum, inputNum),  // Row dimension matches the target layer
      weightsHO(outputNum, hiddenNum), 
      biasH(hiddenNum, 1),            // Biases are column vectors
      biasO(outputNum, 1) 
{
    //assign random values
    weightsIH.randomize();
    weightsHO.randomize();
    biasH.randomize();
    biasO.randomize();
}

//exponents makes all the number positive and the map the between 0 and 1 and the sum is 1
Matrix NeuralNetwork::softmax(const Matrix& m) const {
    Matrix result(m.rows, m.cols);
    float sum = 0.0f;

    // Calculate the exponent of each element and find the sum
    for (int i = 0; i < m.rows; i++) {
        result.data[i][0] = exp(m.data[i][0]);
        sum += result.data[i][0];
    }

    //Divide each exponent by the total sum to normalize
    for (int i = 0; i < m.rows; i++) {
        result.data[i][0] /= sum;
    }

    return result;
}

// takes a 784x1 vector 28*28
Matrix NeuralNetwork::forward(const Matrix& inputVector) {
    
    // part 1 input to hidden
    
    Matrix hiddenOutputs = Matrix::multiply(weightsIH, inputVector); //multiply the wights to input
    
    hiddenOutputs.add(biasH); // add bias to hidden
    
    
    hiddenOutputs.applyReLU(); //apply ReLU

    // part 2 hidden to output
    
    Matrix finalOutputs = Matrix::multiply(weightsHO, hiddenOutputs); //multiply the weights
    
  
    finalOutputs.add(biasO); //add the biases 
    
    return softmax(finalOutputs); // return by apply softmax
}

void NeuralNetwork::train(const Matrix& inputVector, int targetLabel, float learningRate) {
  

    //keep track of the intermediate forward propagation values
    Matrix hiddenRaw = Matrix::multiply(weightsIH, inputVector);
    hiddenRaw.add(biasH);
    
    Matrix hiddenOutputs = hiddenRaw; //keep a copy
    hiddenOutputs.applyReLU();

    Matrix finalRaw = Matrix::multiply(weightsHO, hiddenOutputs);
    finalRaw.add(biasO);
    Matrix finalOutputs = softmax(finalRaw);

   //create a new 10x1 matrix will target values
    Matrix targets(outputNodes, 1, 0.0f);
    targets.data[targetLabel][0] = 1.0f;

//calculate error
    Matrix outputError(outputNodes, 1);
    for (int i = 0; i < outputNodes; i++) {
        outputError.data[i][0] = finalOutputs.data[i][0] - targets.data[i][0];
    }

    //backpropagation


    // Hidden Error = (WeightsHO^T) * OutputError
    Matrix weightsHO_T = Matrix::transpose(weightsHO); //transpose to change directin Ouput to hidden
    Matrix hiddenError = Matrix::multiply(weightsHO_T, outputError);

   
    Matrix reluGradient = hiddenRaw.deriveReLU(); // if neutron was positive 1 and 0 if it was negative
    hiddenError.multiplyElementWise(reluGradient); // element wise, so we know which errors acutally contributed


    
    // update hidden to output
    
    Matrix hiddenOutputs_T = Matrix::transpose(hiddenOutputs); //Delta WeightsHO =OutputError*(HiddenOutputs^T)* LearningRate
    Matrix deltaWeightsHO = Matrix::multiply(outputError, hiddenOutputs_T);
    
    for (int i = 0; i < weightsHO.rows; i++) {
        for (int j = 0; j < weightsHO.cols; j++) {
            weightsHO.data[i][j] -= learningRate * deltaWeightsHO.data[i][j]; //learnign rate is low so as to not run-off
        }
      
        biasO.data[i][0] -= learningRate * outputError.data[i][0]; 
    }

    // update input to error
    
    Matrix inputVector_T = Matrix::transpose(inputVector); // Delta WeightsIH = HiddenError * (InputVector^T) * LearningRate
    Matrix deltaWeightsIH = Matrix::multiply(hiddenError, inputVector_T);

    for (int i = 0; i < weightsIH.rows; i++) {
        for (int j = 0; j < weightsIH.cols; j++) {
            weightsIH.data[i][j] -= learningRate * deltaWeightsIH.data[i][j];
        }
   
        biasH.data[i][0] -= learningRate * hiddenError.data[i][0]; //update biases directly
    }
}




void NeuralNetwork::saveWeights(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file to save weights: " << filename << std::endl;
        return;
    }

    // Save every structural Matrix layer sequentially
    // Change these variable names to match exactly whatever your NN class calls its matrices!
    weightsIH.save(outFile); // Weights: Input to Hidden
    weightsHO.save(outFile); // Weights: Hidden to Output
    biasH.save(outFile);     // Bias: Hidden Layer
    biasO.save(outFile);     // Bias: Output Layer

    outFile.close();
}

bool NeuralNetwork::loadWeights(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // Return false instead of crashing, so main() can handle fresh setups gracefully
        return false; 
    }

    // Load them back in the EXACT same sequence they were saved
    weightsIH.load(inFile);
    weightsHO.load(inFile);
    biasH.load(inFile);
    biasO.load(inFile);

    inFile.close();
    return true;
}