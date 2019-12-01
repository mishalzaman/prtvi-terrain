#include "Heightmap.h"

void Heightmap::process(STImage heightmapData, std::vector<STVertex>& mesh, std::vector<glm::uvec3>& indices, float scale)
{
	// get a width:height ratio in n:1 format
	int ratio_divisor_w = heightmapData.width / heightmapData.height;
	int ratio_divisor_h = heightmapData.height / heightmapData.height;

	// vertices
	for (int row = 0; row <= heightmapData.height; row++)
	{
		for (int column = 0; column <= heightmapData.width; column++)
		{
			STVertex tMesh;

			float x = (float)column / (float)heightmapData.width / ratio_divisor_h;
			float z = (float)row / (float)heightmapData.height / ratio_divisor_w;

			float y;
			if (row == heightmapData.height || column == heightmapData.width || row == 0 || column == 0)
			{
				y = 0.0f;
			}
			else
			{
				float pixel = heightmapData.pixels[heightmapData.width * row + column];
				y = float(pixel / heightmapData.colorDepth)*scale;
			}

			// subtracting 0.5 from x and z to make the resulting x/y coordinate in the range
			// -0.5 and 0.5.
			tMesh.position = glm::vec3(x - 0.5, y, z - 0.5);
			tMesh.normal = glm::vec3(0, 0, 0);
			tMesh.texture = glm::vec2(x, z);
			mesh.push_back(tMesh);
		}
	}

	// indices

	for (int row = 0; row < heightmapData.height; row++)
	{
		int row1 = row * (heightmapData.width + 1);
		int row2 = (row + 1) * (heightmapData.width + 1);

		for (int column = 0; column < heightmapData.width; column++)
		{
			// triangle 1
			indices.push_back(glm::uvec3(row1 + column, row1 + column + 1, row2 + column + 1));

			// triangle 2
			indices.push_back(glm::uvec3(row1 + column, row2 + column + 1, row2 + column));
		}
	}
}

void Heightmap::textures(std::vector<STVertex> &mesh)
{
	for (int i = 0; i < mesh.size(); i++)
	{
		mesh[i].texture = glm::vec2(mesh[i].position.x, mesh[i].position.y);
	}
}

void Heightmap::normals(std::vector<STVertex> &mesh, std::vector<glm::uvec3> &indices)
{
	for (int i = 0; i < indices.size(); i++)
	{
		glm::vec3 v1 = mesh[indices[i].z].position;
		glm::vec3 v2 = mesh[indices[i].y].position;
		glm::vec3 v3 = mesh[indices[i].x].position;

		glm::vec3 edge1 = v1 - v2;
		glm::vec3 edge2 = v1 - v3;
		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

		mesh[indices[i].x].normal += normal;
		mesh[indices[i].y].normal += normal;
		mesh[indices[i].z].normal += normal;
	}
}

int Heightmap::greatest_common_divisor(int a, int b)
{
	// Everything divides 0  
	if (a == 0)
		return b;
	if (b == 0)
		return a;

	// base case 
	if (a == b)
		return a;

	// a is greater 
	if (a > b)
		return Heightmap::greatest_common_divisor(a - b, b);
	return Heightmap::greatest_common_divisor(a, b - a);
}