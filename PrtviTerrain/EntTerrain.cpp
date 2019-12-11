#include "EntTerrain.h"
#include <ctime>

EntTerrain::EntTerrain()
{
	// setup model matrix
	this->model = glm::translate(this->model, this->position);
	this->model = glm::scale(this->model, this->scale);
}

EntTerrain::~EntTerrain() {

}

bool EntTerrain::load(const char * heightmapFilename, const char * diffusemapFilename, const char * normalmapFilename)
{
	std::clock_t begin = clock();
	if (!LdrPGM::load(heightmapFilename, this->heightmapData))
		return false;
	float duration = (clock() - begin) / 1000;
	printf("Load heightmap PGM file duration: %f\n", duration);
	
	begin = clock();
	Heightmap::process(this->heightmapData, this->mesh, this->indices, this->heightScale);
	duration = (clock() - begin) / 1000;
	printf("Load mesh and indices duration: %f\n", duration);

	begin = clock();
	Heightmap::normals(this->mesh, this->indices);
	duration = (clock() - begin) / 1000;
	printf("Load normals duration: %f\n", duration);
	
	begin = clock();
	this->vertexBuffers();
	duration = (clock() - begin);
	printf("Load VBO and VAO duration: %f\n", duration);

	begin = clock();
	if (!this->loadDiffuseMap(diffusemapFilename))
		return false;
	duration = (clock() - begin);
	printf("Load diffuse map duration: %f\n", duration);

	begin = clock();
	if (!this->loadNormalMap(normalmapFilename))
		return false;
	duration = (clock() - begin);
	printf("Load normal map duration: %f\n", duration);

	return true;
}

void EntTerrain::increaseHeightScale() {
	this->heightScale += 0.01f;
}

void EntTerrain::decreaseHeightScale() {
	this->heightScale -= 0.01f;
}

void EntTerrain::draw(glm::mat4& projection, glm::mat4& view, Shader& shader, glm::vec3 lightPosition, glm::vec3 viewPos) {
	
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);

	glBindTexture(GL_TEXTURE_2D, this->diffuseTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->diffuseTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->normalmapTexture);

	shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
	shader.setFloat("scale", this->heightScale);
	shader.setVec3("viewPos", viewPos);

	glBindVertexArray(this->VAO);
	shader.setMat4("model", this->model);

	// light properties
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("lightPos", lightPosition);
	shader.setVec3("objectColor", glm::vec3(0.6f, 0.6f, 0.6f));

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size() * 3, GL_UNSIGNED_INT, 0);
}

void EntTerrain::vertexBuffers() {
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

	// tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(STVertex), (void*)offsetof(STVertex, tangent));
	glEnableVertexAttribArray(3);

	// bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(STVertex), (void*)offsetof(STVertex, bitangent));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

bool EntTerrain::loadDiffuseMap(const char* filename)
{
	glGenTextures(1, &this->diffuseTexture);
	glBindTexture(GL_TEXTURE_2D, this->diffuseTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	this->diffusemapData = LdrStbiWrapper::load(filename, width, height, nrChannels, LdrStbiWrapper::STBI_rgb_alpha);
	if (this->diffusemapData)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->diffusemapData);
		glGenerateMipmap(GL_TEXTURE_2D);
		LdrStbiWrapper::free(this->diffusemapData);
		return true;
	}
	else
	{
		LdrStbiWrapper::free(this->diffusemapData);
		return false;
	}
}

bool EntTerrain::loadNormalMap(const char* filename)
{
	glGenTextures(1, &this->normalmapTexture);
	glBindTexture(GL_TEXTURE_2D, this->normalmapTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;

	this->normalmapData = LdrStbiWrapper::load(filename, width, height, nrChannels, LdrStbiWrapper::STBI_default);
	if (this->normalmapData)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, this->normalmapData);
		glGenerateMipmap(GL_TEXTURE_2D);
		LdrStbiWrapper::free(this->normalmapData);
		return true;
	}
	else
	{
		LdrStbiWrapper::free(this->normalmapData);
		return false;
	}
}
