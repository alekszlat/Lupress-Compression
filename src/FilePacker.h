#pragma once
#include "Node.h"
#include<string>
#include<map>
#include<queue>
using namespace std;

struct Compare {
	bool operator()(Node* a, Node* b) {
		return a->frequincy > b->frequincy; // For min-heap
	}
};

class FilePacker {
private: 
	string filename; // Replace with your file name
	string text;
	string saveText;
	map<char, int> charCount;
	priority_queue<Node*, vector<Node*>, Compare> pq;
	Node* root;
	map<char, string> huffmanCodes;
	map<string, char> decodeCodes;
public:
	FilePacker(string);
	void freeTree(Node*);
	bool fileExists(const string&) const;
	void deleteFile(const string&) const;
	string extractTextFromFile() const;
	void generateFrequencies();
	void generateCode();
	void generateCodes(Node* root, string str, map<char, string>& huffmanCodes, map<string, char>& decodeCodes);
	string compressText(string text);
	void compressToBinary();
	void saveCodeTable(string);
	void showBytes();
	void decompressToText();
	void loadCodeTable(string);
};

