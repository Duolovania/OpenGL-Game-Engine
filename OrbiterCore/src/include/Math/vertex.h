#pragma once
#include "vector.h"
#include "glm/glm.hpp"
#include "gtc/matrix_transform.hpp"

struct Vertex
{
	glm::vec3 Position;
	Vector4 Color;
	Vector2 TextureCoords;
	float TextureID;
};