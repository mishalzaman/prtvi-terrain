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

bool Terrain::loadVertices(const char * heightmapFilename, const char * diffusemapFilename, const char * normalMapFilename)
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
	
	return true;
}

bool Terrain::loadOGLBuffers(const char* diffusemapFilename, const char* normalMapFilename)
{
	std::clock_t begin = clock();

	begin = clock();
	OglGenVertexAttributes::generate(this->VAO, this->VBO, this->mesh, this->indices, 3);
	float duration = (clock() - begin) / 1000;
	printf("Load VBO and VAO duration: %f\n", duration);

	begin = clock();
	if (!OglGenTexture::bind2dLinear(diffusemapFilename, this->diffuseTextureID))
		return false;
	duration = (clock() - begin);
	printf("Load diffuse map duration: %f\n", duration);

	begin = clock();
	if (!OglGenTexture::bind2dLinear(normalMapFilename, this->normalMapTextureID))
		return false;
	duration = (clock() - begin);
	printf("Load normal map duration: %f\n", duration);

	return true;
}

void Terrain::increaseHeightScale() {
	this->heightScale += 0.01f;
}

void Terrain::decreaseHeightScale() {
	this->heightScale -= 0.01f;
}

int Terrain::verticesCount()
{
	return this->mesh.size();
}

int Terrain::indicesCount()
{
	return this->indices.size();
}

void Terrain::draw(glm::mat4& projection, glm::mat4& view, Shader& shader, glm::vec3 lightPosition, glm::vec3 viewPos) {

	shader.use();

	// assign textures to sampler2D names
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);

	// activate textures to ID
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->diffuseTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->normalMapTextureID);

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
	shader.setFloat("scale", this->heightScale);
	shader.setMat4("model", this->model);
	shader.setVec3("viewPos", viewPos);

	// light properties
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("lightPos", lightPosition);
	shader.setVec3("objectColor", glm::vec3(0.6f, 0.6f, 0.6f));

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size() * 3, GL_UNSIGNED_INT, 0);
}