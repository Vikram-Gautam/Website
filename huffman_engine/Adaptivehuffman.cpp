#include <bits/stdc++.h>
#include "Adaptivehuffman.h"

using namespace std;


AdaptiveHuffman::AdaptiveHuffman() { //constructor initializes the tree with NYT node having the maximum priority
    maxNodeNumber = 512; 
    nyt = new HuffmanNode('\0', 0, maxNodeNumber);
    root = nyt;
    
    nodeOrder.resize(513, nullptr);
    nodeOrder[maxNodeNumber] = nyt;
}


void AdaptiveHuffman::clearTree(HuffmanNode* current) { // helper function to clear memory
    if (!current) return;
    clearTree(current->left);
    clearTree(current->right);
    delete current;
}


AdaptiveHuffman::~AdaptiveHuffman() { //destructor
    clearTree(root);
}


HuffmanNode* AdaptiveHuffman::findHighestNodeInBlock(int weight) { // locates the highest node in a weight class
    HuffmanNode* highestNode = nullptr;
    int highestNum = -1;

    
    for (int i = 0; i <= 512; i++) {
        if (nodeOrder[i] && nodeOrder[i]->weight == weight) {
            
            if (nodeOrder[i]->nodeNumber > highestNum) {
                highestNum = nodeOrder[i]->nodeNumber;
                highestNode = nodeOrder[i];
            }
        }
    }
    return highestNode;
}

void AdaptiveHuffman::swapNodes(HuffmanNode* n1, HuffmanNode* n2) { //swap two nodes
    
    swap(nodeOrder[n1->nodeNumber], nodeOrder[n2->nodeNumber]); // change the look up vector
    
    
    swap(n1->nodeNumber, n2->nodeNumber); // change the nodes themselfs
    
   // switch the child of parents
    if (n1->parent) {
        if (n1->parent->left == n1) n1->parent->left = n2;
        else n1->parent->right = n2;
    }
    if (n2->parent) {
        if (n2->parent->left == n2) n2->parent->left = n1;
        else n2->parent->right = n1;
    }
    
    // change parents themselves
    swap(n1->parent, n2->parent);
}


void AdaptiveHuffman::getPathToNode(HuffmanNode* target, string& currentPath, string& finalPath) {
    HuffmanNode* curr = target;
    while (curr && curr->parent) {      //traverse from the node to the root and then reverse it
        HuffmanNode* p = curr->parent;
        if (p->left == curr) {
            currentPath += "0";
        } else if (p->right == curr) {
            currentPath += "1";
        }
        curr = p;
    }
    finalPath = currentPath;
    reverse(finalPath.begin(), finalPath.end());
}

// FGK implementation 
void AdaptiveHuffman::updateTree(char c) {
    HuffmanNode* curr = nullptr;

   // character does not exist 
    if (leafMap.find(c) == leafMap.end()) {
        
        HuffmanNode* oldNyt = nyt;
        
        
        HuffmanNode* newLeaf = new HuffmanNode(c, 1, oldNyt->nodeNumber - 1); // new nodes have lesser Node number
        HuffmanNode* newNyt = new HuffmanNode('\0', 0, oldNyt->nodeNumber - 2);

        
        oldNyt->data = '\0'; //restrcutrue the tree
        oldNyt->weight = 1;
        oldNyt->left = newNyt;
        oldNyt->right = newLeaf;
        
        newNyt->parent = oldNyt;
        newLeaf->parent = oldNyt;

                                    //update look ups
        leafMap[c] = newLeaf;
        nodeOrder[newLeaf->nodeNumber] = newLeaf;
        nodeOrder[newNyt->nodeNumber] = newNyt;
        
        nyt = newNyt; // reasign NYT
        curr = oldNyt->parent; // Climb up for adjustments
    } 
    // it exists, store it in current
    else {
        curr = leafMap[c];
    }

    // Climb up, maintianing the sibling property
    while (curr != nullptr) {
        // Evaluate if a higher numbered sibling block component has been bypassed
        HuffmanNode* highestInBlock = findHighestNodeInBlock(curr->weight);
        
        if (highestInBlock && curr != highestInBlock && curr->parent != highestInBlock && highestInBlock->parent != curr) {
            swapNodes(curr, highestInBlock); // swap if they highest is not an internal node
        }

        curr->weight++; // Increment current's weight
        curr = curr->parent; // Climb up
    }
}

// Helper to convert character to 8-bit binary string
string charTo8Bit(char c) {
    bitset<8> bits(c);
    return bits.to_string();
}

// Helper to convert 8-bit binary string back to character
char bitsToChar(const string& bitStr) {
    bitset<8> bits(bitStr);
    return static_cast<char>(bits.to_ulong());
}

// Generates both pure and debug streams simultaneously
pair<string, string> AdaptiveHuffman::encode(const string& input) {
    string pureStream = "";
    string debugStream = "";

    for (char c : input) {
        if (leafMap.find(c) == leafMap.end()) {
            string nytPath = "";
            string finalNytPath = "";
            getPathToNode(nyt, nytPath, finalNytPath);
            
            // Both streams get the path to the NYT node
            pureStream += finalNytPath;
            debugStream += finalNytPath;

            // Pure stream gets the 8-bit ASCII; Debug stream gets [c]
            pureStream += charTo8Bit(c);
            debugStream += "[" + string(1, c) + "]";
        } else {
            string leafPath = "";
            string finalLeafPath = "";
            getPathToNode(leafMap[c], leafPath, finalLeafPath);
            
            pureStream += finalLeafPath;
            debugStream += finalLeafPath;
        }
        updateTree(c);
    }
    return {pureStream, debugStream};
}

// Decodes PURE bitstrings only
string AdaptiveHuffman::decode(const string& input) {
    string decodedText = "";
    HuffmanNode* curr = root; 
    int i = 0;
    int n = input.length();

    while (i < n) {
        if (curr == nyt) {
            // Read the next 8 bits as a raw character literal
            if (i + 8 <= n) {
                string byteStr = input.substr(i, 8);
                char newChar = bitsToChar(byteStr);
                decodedText += newChar;
                
                updateTree(newChar);
                curr = root; 
                i += 8; 
            } else {
                break; 
            }
        } 
        else {
            if (input[i] == '0') {
                curr = curr->left;
            } else if (input[i] == '1') {
                curr = curr->right;
            }
            i++;

            if (curr && curr->left == nullptr && curr->right == nullptr && curr != nyt) {
                decodedText += curr->data;
                updateTree(curr->data);
                curr = root; 
            }
        }
    }
    return decodedText;
}