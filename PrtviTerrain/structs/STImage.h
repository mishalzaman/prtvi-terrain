#ifndef STRUCT_IMAGE_H
#define STRUCT_IMAGE_H

#include <vector>

struct STImage {
	int width;
	int height;
	int colorDepth;
	std::vector<int> pixels;
};

#endif