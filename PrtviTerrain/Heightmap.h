#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include "STVertex.h"
#include "PGM.h"

using namespace std;

namespace Heightmap
{
	void process(
		PGM::Image heightmapData,
		std::vector<STVertex>& mesh,
		std::vector<glm::uvec3> &indices,
		float scale
	);
	void textures(std::vector<STVertex> &mesh);
	void normals(std::vector<STVertex> &mesh, std::vector<glm::uvec3> &indices);
	int greatest_common_divisor(int a, int b);
}

#endif
