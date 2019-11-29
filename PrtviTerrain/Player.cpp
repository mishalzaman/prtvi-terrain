#include "Player.h"



Player::Player()
{
}


Player::~Player()
{
}

void Player::load(glm::vec3 position)
{
	this->position = position;
	this->createSphere(16, 16);

	unsigned int IBO;

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &IBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->mesh.size() * sizeof(STVertex), &this->mesh[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(glm::uvec3), glm::value_ptr(this->indices[0]), GL_STATIC_DRAW);

	// positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(STVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(STVertex), (void*)offsetof(STVertex, normal));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Player::draw(glm::mat4 & projection, glm::mat4 & view, Shader & shader)
{
	shader.use();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	glBindVertexArray(this->VAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, this->position);
	model = glm::scale(model, glm::vec3(0.05, 0.05, 0.05));
	shader.setMat4("model", model);

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size() * 3, GL_UNSIGNED_INT, 0);
}

void Player::createSphere(int stacks, int slices)
{
	const float PI = 3.14;

	// vertices
	for (int i = 0; i <= stacks; ++i)
	{
		float v = (float)i / (float)stacks;
		float phi = v * PI;

		for (int j = 0; j <= slices; ++j)
		{
			float u = (float)j / (float)slices;
			float theta = u * (PI * 2);

			float x = cos(theta) * sin(phi);
			float y = cos(phi);
			float z = sin(theta) * sin(phi);

			STVertex tMesh;

			tMesh.position = glm::vec3(x, y, z);
			mesh.push_back(tMesh);
		}
	}

	// indices
	for (int i = 0; i < slices * stacks + slices; ++i)
	{
		indices.push_back(glm::uvec3(
			i,
			i + slices + 1,
			i + slices
		));

		indices.push_back(glm::uvec3(
			i + slices + 1,
			i,
			i + 1
		));
	}
}
