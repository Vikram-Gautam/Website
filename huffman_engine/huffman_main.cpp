#include <bits/stdc++.h>
#include "Adaptivehuffman.h"
using namespace std;

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        cerr << "Error: Missing arguments. Usage: ./huffman <input_string> <mode>" << endl; //need 3 parameters, error othereise
        return 1; 
    }

    string inputData = argv[1];  //text to encode or decode
    string mode = argv[2];       //mode

    AdaptiveHuffman processor;

    if (mode == "encode") {
        if (!inputData.empty()) {
            pair<string, string> result = processor.encode(inputData);
            //print the bitstring
            cout << result.first << endl;
        }
    } 
    else if (mode == "decode") {
        if (!inputData.empty()) {
            string decoded = processor.decode(inputData);
            //print the string
            cout << decoded << endl;
        }
    } 
    else {
        cerr << "Error: Invalid mode selection. Use 'encode' or 'decode'." << endl; //invalid mode error
        return 1;
    }

    return 0;
}