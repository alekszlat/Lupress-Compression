#include "FilePacker.h"
#include <iostream>
#include <fstream>

FilePacker::FilePacker(string filename) : filename(filename) {
	root = nullptr;
}

void FilePacker::freeTree(Node* node) {
	if (node == nullptr) {
		return;
	}
	freeTree(node->left);
	freeTree(node->right);
	delete node;
}

bool FilePacker::fileExists(const string& filename) const {
	ifstream file(filename);
	return file.good();
}

void FilePacker::deleteFile(const string& filename) const {
	if (remove(filename.c_str()) != 0) {
		cerr << "Error deleting file: " << filename << endl;
	}
	else {
		cout << "File deleted successfully: " << filename << endl;
	}
}

string FilePacker::extractTextFromFile() const {
	ifstream file(filename);
	string text = "";
	char ch;
	if (!file.is_open()) {
		cerr << "Error opening file: " << filename << endl;
		return "";
	}
	while (file.get(ch)) {
		text += ch; // Add newline character to preserve line breaks
	}
	file.close();
	return text;
}

void FilePacker::generateFrequencies() {
	charCount.clear(); // Clear previous frequencies
	freeTree(root); // Free the previous tree
	while (!pq.empty()) pq.pop(); // Clear the priority queue
	for (char ch : text) {
		charCount[ch]++;
	}

	for (auto pair : charCount) {
		Node* newNode = new Node(pair.first, pair.second);
		pq.push(newNode);
	}

	while (pq.size() > 1) {
		Node* left = pq.top();
		pq.pop();

		Node* right = pq.top();
		pq.pop();

		// Create new parent node
		Node* parent = new Node('\0', left->frequincy + right->frequincy);
		parent->left = left;
		parent->right = right;

		pq.push(parent);
	}
	root = pq.top();
}

void FilePacker::generateCode() {
	string str = "";
	huffmanCodes.clear(); // Clear previous codes
	decodeCodes.clear(); // Clear previous decode codes
	generateCodes(root, str, huffmanCodes, decodeCodes);
}

void FilePacker::generateCodes(Node* root, string str, map<char, string>& huffmanCodes, map<string, char>& decodeCodes) {
	if (root == nullptr) {
		return;
	}
	if (root->left == nullptr && root->right == nullptr) {
		huffmanCodes[root->ch] = str;
		decodeCodes[str] = root->ch;
	}
	generateCodes(root->left, str + "0", huffmanCodes, decodeCodes);
	generateCodes(root->right, str + "1", huffmanCodes, decodeCodes);
}

string FilePacker::compressText(string text) {
	string compressedText = "";
	for (char ch : text) {
		compressedText += huffmanCodes[ch];
	}
	return compressedText;
}

void FilePacker::compressToBinary() {
	text = extractTextFromFile(); // Read original text
	generateFrequencies();
	generateCode();
	saveCodeTable(filename + ".mt"); // Save Huffman codes to file
	string compressedText = compressText(text); // Get encoded binary string

	ofstream outFile(filename, ios::binary);

	// Write total number of valid bits first (as 4-byte int)
	int totalBits = compressedText.size();
	outFile.write(reinterpret_cast<char*>(&totalBits), sizeof(totalBits));

	unsigned char byte = 0;
	int bitCount = 0;

	for (char ch : compressedText) {
		byte <<= 1;
		if (ch == '1') byte |= 1;
		bitCount++;

		if (bitCount == 8) {
			outFile.write(reinterpret_cast<char*>(&byte), sizeof(byte));
			byte = 0;
			bitCount = 0;
		}
	}

	if (bitCount > 0) {
		byte <<= (8 - bitCount); // Padding with 0s
		outFile.write(reinterpret_cast<char*>(&byte), sizeof(byte));
	}

	outFile.close();
}

void FilePacker::showBytes() {
	ifstream inFile(filename, ios::binary);
	char byte;
	while (inFile.read(&byte, 1)) {
		for (int i = 7; i >= 0; i--) {
			cout << ((byte >> i) & 1);
		}
		cout << " ";
	}
	inFile.close();
}

void FilePacker::decompressToText() {
	loadCodeTable(filename + ".mt"); // Load Huffman codes from file
	deleteFile(filename + ".mt"); // Delete the original file
	ifstream inFile(filename, ios::binary);

	int totalBits = 0;
	inFile.read(reinterpret_cast<char*>(&totalBits), sizeof(totalBits)); // Read bit count

	string compressedText = "";
	char byte;
	int bitsRead = 0;

	while (inFile.read(&byte, 1) && bitsRead < totalBits) {
		for (int i = 7; i >= 0 && bitsRead < totalBits; i--) {
			compressedText += ((byte >> i) & 1) ? '1' : '0';
			bitsRead++;
		}
	}

	inFile.close();

	// Decode the bit string using decodeCodes
	string newText = "";
	string code = "";
	for (char ch : compressedText) {
		code += ch;
		if (decodeCodes.find(code) != decodeCodes.end()) {
			newText += decodeCodes[code];
			code = "";
		}
	}

	ofstream outfile(filename);
	outfile << newText;
	outfile.close();
}

void FilePacker::saveCodeTable(string filename) {
	ofstream outFile(filename);
	for (auto pair : huffmanCodes) {
		char ch = pair.first;
		string code = pair.second;

		// Handle special characters with escape notation
		if (ch == '\n') outFile << "/n:" << code << endl;
		else if (ch == '\t') outFile << "/t:" << code << endl;
		else outFile << ch << ":" << code << endl;

	}
	outFile.close();
}

void FilePacker::loadCodeTable(string filename) {
	ifstream inFile(filename);
	string line;
	while (getline(inFile, line)) {
		if (line.length() < 3) {
			cerr << "Invalid line in code table: " << line << endl;
			continue;
		}

		string key;
		if (line[1] != ':') {
			key = line.substr(0, 2);
		}
		else{
			key = line.substr(0, 1);
		}
		char ch;
		string code;
		if (key == "/n") {
			ch = '\n';
			code = line.substr(3);
		}
        else if (key == "/t") { 
           ch = '\t'; 
           code = line.substr(3); 
        }
		else ch = key[0]; code = line.substr(2);

		//huffmanCodes[ch] = code;
		decodeCodes[code] = ch;
	}
	inFile.close();
}
