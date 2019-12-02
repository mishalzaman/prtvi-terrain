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
#include "STImage.h"

using namespace std;

class Terrain
{
    public:
        Terrain();
        ~Terrain();
		bool load(const char* heightmapFilename, const char* diffusemapFilename);
        void draw(glm::mat4& projection, glm::mat4& view, Shader& shader, glm::vec3 lightPosition);
		void increaseHeightScale();
		void decreaseHeightScale();
    private:
        unsigned int VAO;
        unsigned int VBO;
		glm::mat4 model			 = glm::mat4(1.0f);
		glm::vec3 position		 = glm::vec3(0, 0, 0);
		glm::vec3 scale			 = glm::vec3(1, 1, 1);
        std::vector<STVertex> mesh;
	    std::vector<glm::uvec3> indices;
		void vertexBuffers();

		// heightmap
		STImage heightmapData;
        
		float heightScale = 0.5f;

		// diffuse map
		unsigned char* diffusemapData;
		unsigned int diffuseTexture;
		bool loadDiffuseMap(const char* filename);
};

#endif TERRAIN_H