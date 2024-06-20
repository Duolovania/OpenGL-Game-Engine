#pragma once

struct Vector2
{
	public:
		float x, y;
};

struct Vector3
{
	float x, y, z;
};

struct Vector4
{
	float x, y, z, w;
};

struct Vertex
{
	Vector3 Position;
	Vector4 Color;
	Vector2 TextureCoords;
	float TextureID;
};