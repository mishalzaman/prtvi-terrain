#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <vector>
#include "STVertex.h"

class Light
{
public:
	Light();
	~Light();
	void load(glm::vec3 position);
	void draw(glm::mat4& projection, glm::mat4& view, Shader& shader);
	glm::vec3 position = glm::vec3(0, 0, 0);
	void up();
	void down();
	void forward();
	void backward();
	void right();
	void left();
private:
	unsigned int VAO;
	unsigned int VBO;
	std::vector<STVertex> mesh;
	std::vector<glm::uvec3> indices;
	void createSphere(int stacks, int slices);
};

#endif

