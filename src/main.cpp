#include "FilePacker.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: mycompressor <compress|decompress> <filename>\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string filename = argv[2];

    FilePacker packer(filename);

    if (mode == "compress") {
        packer.compressToBinary();
        std::cout << "Compression completed.\n";
    }
    else if (mode == "decompress") {
        packer.decompressToText();
        std::cout << "Decompression completed.\n";
    }
    else {
        std::cerr << "Invalid mode: " << mode << "\n";
        return 1;
    }

    return 0;
}