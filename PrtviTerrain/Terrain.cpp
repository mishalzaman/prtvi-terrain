#include "Terrain.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

Terrain::Terrain(const char* heightmap)
{
	PGM::load(heightmap, this->heightmapData);
}

Terrain::~Terrain() {

}

void Terrain::load(glm::vec3 position) {
	this->position = position;
	this->model = glm::translate(this->model, this->position);

	Heightmap::process(this->heightmapData, this->mesh, this->indices, this->heightScale);
	Heightmap::normals(this->mesh, this->indices);
    this->processVertices();
}

void Terrain::increaseHeightScale() {
	this->heightScale += 0.01f;
}

void Terrain::decreaseHeightScale() {
	this->heightScale -= 0.01f;
}

void Terrain::draw(glm::mat4& projection, glm::mat4& view, Shader& shader, glm::vec3 lightPosition) {
	printf("%f\n", this->heightScale);
	glBindTexture(GL_TEXTURE_2D, this->diffuseTexture);
	shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
	shader.setFloat("scale", this->heightScale);

	glBindVertexArray(this->VAO);

	shader.setMat4("model", this->model);

	// light properties
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("lightPos", lightPosition);
	shader.setVec3("objectColor", glm::vec3(0.6f, 0.6f, 0.6f));

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size() * 3, GL_UNSIGNED_INT, 0);
}

void Terrain::processVertices() {
    unsigned int IBO;

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &IBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
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

	// textures
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(STVertex), (void*)offsetof(STVertex, texture));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Terrain::diffuseMap(const char* filename)
{
	glGenTextures(1, &this->diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, this->diffuseTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.

	this->diffusemapData = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (this->diffusemapData)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->diffusemapData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load diffuse texture" << std::endl;
	}
}
