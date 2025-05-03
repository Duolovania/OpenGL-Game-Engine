#pragma once
#include "Math/transform.h"
#include "Components/component.h"
#include "Components/spriterenderer.h"
#include "glm/glm.hpp"
#include <vector>
#include <memory>

class GameObject
{
	public:
		virtual ~GameObject() = default;

		GameObject();

		Transform transform;
		std::string objectName;

		glm::mat4 GetView() const; // Gets the position and rotation values from the transform matrix.

		bool HasComponent(const std::string targetComponent) const; // Checks if the object has a specified component.

		// Gets the component of a specified type in the object.
		template<typename T>
		std::shared_ptr<T> GetComponent()
		{
			// Loops through each component.
			for (const auto& component : m_components)
			{
				// Checks if the component types match.
				if (std::shared_ptr<T> comp = dynamic_pointer_cast<T>(component))
				{
					return comp; // Return the component.
				}
			}

			return nullptr;
		}

		// Adds a new component.
		template<typename T>
		void AddComponent(T newComponent)
		{
			m_components.push_back(std::make_unique<T>(newComponent));
		}
		
		//void AddComponent(Component newComponent);
		void RemoveComponent(std::string componentName);

	private:
		std::vector<std::shared_ptr<Component>> m_components;
};