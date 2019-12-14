#include "Terrain.h"
#include <ctime>

Terrain::Terrain()
{
	// setup model matrix
	this->model = glm::translate(this->model, this->position);
	this->model = glm::scale(this->model, this->scale);
}

Terrain::~Terrain() {

}

bool Terrain::load(const char * heightmapFilename, const char * diffusemapFilename)
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
	OglGenVertexAttributes::generate(this->VAO, this->VBO, this->mesh, this->indices, 4);
	duration = (clock() - begin);
	printf("Load VBO and VAO duration: %f\n", duration);

	begin = clock();
	if (!this->loadDiffuseMap(diffusemapFilename))
		return false;
	duration = (clock() - begin);
	printf("Load diffuse map duration: %f\n", duration);

	return true;
}

void Terrain::increaseHeightScale() {
	this->heightScale += 0.01f;
}

void Terrain::decreaseHeightScale() {
	this->heightScale -= 0.01f;
}

void Terrain::draw(glm::mat4& projection, glm::mat4& view, Shader& shader, glm::vec3 lightPosition) {
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

bool Terrain::loadDiffuseMap(const char* filename)
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
