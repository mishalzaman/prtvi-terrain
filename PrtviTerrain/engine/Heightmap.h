#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <STVertex.h>
#include <STImage.h>
#include <LdrPGM.h>

using namespace std;

namespace Heightmap
{
	bool process(
		STImage heightmapData,
		std::vector<STVertex>& mesh,
		std::vector<glm::uvec3> &indices,
		float scale
	);
	void textures(std::vector<STVertex> &mesh);
	void normals(std::vector<STVertex> &mesh, std::vector<glm::uvec3> &indices);
	int greatest_common_divisor(int a, int b);
}

#endif
