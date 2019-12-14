#ifndef RNDR_TEXT_H
#define RNDR_TEXT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Shader.h>

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class RndrText
{
public:
	RndrText();
	~RndrText();
	void load(int screenWidth, int screenHeight);
	void renderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
private:
	int screenWidth;
	int screenHeight;
	GLuint VAO;
	GLuint VBO;
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	FT_Library ft;
	FT_Face face;
	std::map<GLchar, Character> Characters;
	void loadBuffers();
};

#endif

