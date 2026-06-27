#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Matrix.h"
#include "NeuralNetwork.h"
#include "MNISTLoader.h"
using namespace std;

int runEval(NeuralNetwork& nn, const vector<Matrix>& images, const vector<int>& labels, bool trainMode, float lr) {
    int correct = 0;
    for (size_t i = 0; i < images.size(); i++) {
        Matrix output = nn.forward(images[i]);
        int pred = 0;
        float max_prob = -1.0f;
        for (int d = 0; d < 10; d++) {
            if (output.data[d][0] > max_prob) {
                max_prob = output.data[d][0];
                pred = d;
            }
        }
        if (pred == labels[i]) correct++;
        if (trainMode) nn.train(images[i], labels[i], lr);
    }
    return correct;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Error: Missing arguments. Usage: ./digit_nn <mode> ..." << endl;
        return 1;
    }

    string mode = argv[1];
    NeuralNetwork nn(784, 128, 10);

  //training mode
    if (mode == "train") {
        if (argc < 4) {
            cerr << "Error: Usage: ./digit_nn train <epochs> <learning_rate>" << endl;
            return 1;
        }

        // Parse custom training parameters passed from the website inputs
        int epochs = stoi(argv[2]);
        float lr = stof(argv[3]);

        // Try to load existing weights first so training is cumulative, 
        // otherwise it will train from a fresh random initialization
        nn.loadWeights("weights.txt");

        auto trainImg = loadMNISTImages("train-images.idx3-ubyte");
        auto trainLab = loadMNISTLabels("train-labels.idx1-ubyte");

        // Loop through epochs and print accuracy strings that Python can intercept
        for (int epoch = 1; epoch <= epochs; epoch++) {
            int correct = runEval(nn, trainImg, trainLab, true, lr);
            float acc = (static_cast<float>(correct) / trainImg.size()) * 100.0f;
            
            // This explicit print statement is what the website UI will display
            cout << "Epoch " << epoch << " | Train Acc: " << fixed << setprecision(2) << acc << "%" << endl;
        }

        // Overwrite the weights file with the newly optimized parameters
        nn.saveWeights("weights.txt"); 
        return 0;
    }

        // predicting mode
    else if (mode == "predict") {
        if (argc < 3) {
            cerr << "Error: Missing pixel data string." << endl;
            return 1;
        }

        // Instantly load the most recently saved weights text file
        if (!nn.loadWeights("weights.txt")) {
            cerr << "Error: weights.txt not found!" << endl;
            return 1;
        }

        string pixelString = argv[2];
        stringstream ss(pixelString);
        
        Matrix imgMatrix(784, 1);
        double val;
        int idx = 0;
        
        while (ss >> val && idx < 784) {
            imgMatrix.data[idx][0] = val / 255.0; 
            idx++;
        }

        Matrix output = nn.forward(imgMatrix);
        
        int pred = 0;
        float max_prob = -1.0f;
        for (int d = 0; d < 10; d++) {
            if (output.data[d][0] > max_prob) {
                max_prob = output.data[d][0];
                pred = d;
            }
        }

        cout << pred << endl;
    }
    
    return 0;
}