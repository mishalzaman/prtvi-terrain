#ifndef STRUCT_VERTEX_H 
#define STRUCT_VERTEX_H

#include <glm/glm.hpp>

struct STVertex {
	glm::vec3 position;
	glm::vec3 normal = glm::vec3(0, 0, 0);
	glm::vec2 texture;
};
#endif