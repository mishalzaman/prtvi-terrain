#ifndef LDR_PGM_H
#define LDR_PGM_H

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <STImage.h>

namespace LdrPGM {
	struct Image {
		int width;
		int height;
		int colorDepth;
		std::vector<int> pixels;
	};

	bool load(const char* filename, STImage& image);
};

#endif
