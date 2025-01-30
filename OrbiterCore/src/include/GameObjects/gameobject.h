#pragma once
#include "Math/transform.h"
#include "Components/component.h"
#include "glm/glm.hpp"
#include <vector>

class GameObject
{
	public:
		virtual ~GameObject() = default;

		Transform transform;
		std::string objectName;

		glm::mat4 GetView() const;
		
		void AddComponent(Component newComponent);
		void RemoveComponent(std::string componentName);

	private:
		std::vector<Component> m_components;
};