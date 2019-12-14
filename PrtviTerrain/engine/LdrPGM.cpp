#include "LdrPGM.h"

bool LdrPGM::load(const char * filename, STImage& image)
{
	std::ifstream infile(filename);
	std::stringstream ss;
	std::string inputLine = "";

	if (!infile) {
		std::cout << "Error openg PGM file" << std::endl;
		infile.close();
		return false;
	}

	// Version
	std::getline(infile, inputLine);
	if (inputLine.compare("P2") != 0) {
		std::cerr << "Version error" << std::endl;
		infile.close();
		return false;
	}

	// Comment
	std::getline(infile, inputLine);

	// size
	ss << infile.rdbuf();
	ss >> image.width >> image.height;
	ss >> image.colorDepth;

	// pixels
	int numPixels = image.width * image.height;
	int el = 0;
	for (int pixel = 0; pixel < numPixels; pixel++) {
		ss >> el;
		image.pixels.push_back(el);
	}

	infile.close();

	return true;
}
