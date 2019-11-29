#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Heightmap.h"
#include "Shader.h"
#include <iostream>
#include "STVertex.h"
#include "PGM.h"

using namespace std;

class Terrain
{
    public:
        Terrain(const char* heightmap);
        ~Terrain();
        void load(glm::vec3 position);
        void draw(glm::mat4& projection, glm::mat4& view, Shader& shader, glm::vec3 lightPosition);
		void increaseHeightScale();
		void decreaseHeightScale();
		void diffuseMap(const char* filename);
		glm::vec3 position = glm::vec3(0, 0, 0);
    private:
        unsigned int VAO;
        unsigned int VBO;
		glm::mat4 model = glm::mat4(1.0f);
        std::vector<STVertex> mesh;
	    std::vector<glm::uvec3> indices;

		// heightmap
		PGM::Image heightmapData;
        void processVertices();
		float heightScale = 0.5f;

		// diffuse map
		unsigned char* diffusemapData;
		unsigned int diffuseTexture;
		
};

#endif TERRAIN_H