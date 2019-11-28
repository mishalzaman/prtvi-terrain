#ifndef PGM_H
#define PGM_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace PGM {
	struct Image {
		int width;
		int height;
		int colorDepth;
		std::vector<int> pixels;
	};

	bool load(const char* filename, PGM::Image& image);
};

#endif
