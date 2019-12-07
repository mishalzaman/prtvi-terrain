#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <string>
#include "Shader.h"
#include "LdrStbiWrapper.h"

class Shader;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();
	void load();
	void draw(glm::mat4& projection, glm::mat4& view, Shader& shader);
private:
	void loadCubeMap();
	void loadCube();
	unsigned int textureID;
	unsigned int VAO;
	unsigned int VBO;
	std::vector<std::string> faces = {
		"assets/skybox/right.jpg",
		"assets/skybox/left.jpg",
		"assets/skybox/top.jpg",
		"assets/skybox/bottom.jpg",
		"assets/skybox/front.jpg",
		"assets/skybox/back.jpg"
	};
	void cleanup();
};

#endif