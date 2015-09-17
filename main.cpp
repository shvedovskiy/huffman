#include <iostream>
#include <fstream>
#include <string>

#include <vector>
#include <list>
#include <map>

class Node {
public:
    int _number;
    char _symbol;
    Node *_left, *_right;
    
    Node() { }
    Node(Node* l, Node* r) : _left(l), _right(r) { // from sons
        _number = l->_number + r->_number;
    }
    friend std::less<Node>;
};

class CompareSons {
public:
    bool operator() (const Node* left, const Node* right) const {
        return left->_number < right->_number;
    }
};

void printTree(Node* root, size_t k = 1) {
    if (root != nullptr) {
        printTree(root->_left, k+3);
        for (size_t i = 0; i != k; ++i)
            std::cout << " ";
        if (root->_symbol)
            std::cout << root->_number << " ("
            << root->_symbol << ")" << std::endl;
        else
            std::cout << root->_number << std::endl;
        printTree(root->_right, k+3);
    }
}

std::map<char, std::vector<bool>> buildTable(Node* root) {
    std::vector<bool> code;
    std::map<char, std::vector<bool> > table;
    if (root->_left != nullptr) {
        code.push_back(0);
        buildTable(root->_left);
    }
    if (root->_right != nullptr) {
        code.push_back(1);
        buildTable(root->_right);
    }
    if (root->_left == nullptr && root->_right == nullptr)
        table[root->_symbol] = code;
    code.pop_back();
    return table;
}

std::map<char, std::vector<bool>> encode(char* filename) {
    std::map<char, int> map_cnt;
    std::list<Node*> list_of_pairs;
    std::ifstream infile(filename);
    
    while(!infile.eof()) {
        char ch = infile.get();
        map_cnt[ch]++;
    }
    
    for (std::map<char, int>::iterator it = map_cnt.begin(); it != map_cnt.end(); ++it) {
        Node* pNode = new Node();
        pNode->_symbol = it->first;
        pNode->_number = it->second;
        list_of_pairs.push_back(pNode);
    }
    
    while (list_of_pairs.size() != 1) {
        list_of_pairs.sort(CompareSons());
        Node* sonleft = list_of_pairs.front();
        list_of_pairs.pop_front();
        Node* sonright = list_of_pairs.front();
        list_of_pairs.pop_front();
        
        Node* parent = new Node(sonleft, sonright);
        list_of_pairs.push_back(parent);
    }
    
    Node* root = list_of_pairs.front();
    std::map<char, std::vector<bool>> table = buildTable(root);
    
    for (auto it = table.begin(); it != table.end(); ++it)
    {
        std::cout << it->first << ": ";
        std::vector<bool>& vec = it->second;
        for (std::vector<bool>::iterator itt = vec.begin(); itt != vec.end(); ++itt)
            std::cout << *itt << " ";
        std::cout << std::endl;
    }
    
    infile.close();
    return table;
}

void decode(std::map<char, std::vector<bool>> table, char* inputfilename, char* outputfilename) {
    std::ifstream infile(inputfilename);
    std::ofstream outfile(outputfilename);
    
    int cnt = 0;
    char buf = 0; // 8 zeros
    
    while (!infile.eof()) {
        char ch = infile.get();
        std::vector<bool> bites = table[ch];
        for (int j = 0; j != bites.size(); ++j) {
            buf = buf | bites[j] << (7 - cnt);
            cnt++;
            if (cnt == 8) {
                cnt = 0;
                outfile << buf;
                buf = 0;
            }
        }
    }
    infile.close();
    outfile.close();
}

int main() {
    std::map<char, std::vector<bool>> table = encode("1.txt");
    decode(table, "1.txt", "output.bin");
}

