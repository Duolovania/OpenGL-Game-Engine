#pragma once
#include "Math/transform.h"
#include <string>

class GameObject
{
	public:
		virtual ~GameObject() = default;

		Transform transform;
		std::string objectName;
};