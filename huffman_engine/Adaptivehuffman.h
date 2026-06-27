#ifndef ADAPTIVE_HUFFMAN_H // check if this file has been defined previously 
#define ADAPTIVE_HUFFMAN_H

#include <string>
#include <vector>
#include <utility>
#include <unordered_map> // can't use namespace as it will cause "Namespace Poisoning", apparently it will force all my files to use it, when i incldue this header. But i was goint to use it either ways. but then again better be safe than sorry :)

// The actual node which will store the data
struct HuffmanNode {
    char data;                 // Characters, blank for internal noods
    int weight;                // Frequency, idk why they are called weights, but all the theory calls them weights so I will also call them weights
    int nodeNumber;            // Node number tells me the priorities, it goes from left to right and bottom to top
    HuffmanNode* left;
    HuffmanNode* right;
    HuffmanNode* parent;       

    HuffmanNode(char d, int w, int num) // basic consturcgor
        : data(d), weight(w), nodeNumber(num), left(nullptr), right(nullptr), parent(nullptr) {}
};

class AdaptiveHuffman {
private:
    HuffmanNode* root;          // points at the root obv
    HuffmanNode* nyt;          // points to the not yet constructed node
    int maxNodeNumber;         // will count down from 512
    
    //look ups
    std::unordered_map<char, HuffmanNode*> leafMap; // O(1) look for any char
    std::vector<HuffmanNode*> nodeOrder; // to track node pointers using node index

    // Internal execution helpers
    void updateTree(char c);
    void swapNodes(HuffmanNode* n1, HuffmanNode* n2);
    HuffmanNode* findHighestNodeInBlock(int weight);
    void getPathToNode(HuffmanNode* target, std::string& currentPath, std::string& finalPath);

public:
    AdaptiveHuffman();
    ~AdaptiveHuffman();
    
   std::pair<std::string, std::string> encode(const std::string& input);
   std::string decode(const std::string& input);
    void clearTree(HuffmanNode* current);
};

#endif // end of the if condition 