#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "STVertex.h"
#include "Shader.h"

class Player
{
public:
	Player();
	~Player();
	void load(glm::vec3 position);
	void draw(glm::mat4 & projection, glm::mat4 & view, Shader & shader);
private:
	unsigned int VBO;
	unsigned int VAO;
	glm::vec3 position;
	std::vector<STVertex> mesh;
	std::vector<glm::uvec3> indices;
	void createSphere(int stacks, int slices);
};

#endif

