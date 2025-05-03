#include "GameObjects/gameobject.h"
#include "gtc/matrix_transform.hpp"

GameObject::GameObject()
{
	objectName = "New GameObject";
}

// Gets the position and rotation values from the transform matrix.
glm::mat4 GameObject::GetView() const
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(-transform.position.x, -transform.position.y, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

// Checks if the object has a specified component.
bool GameObject::HasComponent(const std::string targetComponent) const
{
	// Loops through each component in the vector.
	for (int i = 0; i < m_components.size(); i++)
	{
		// Checks if the selected component has the same name as the search term.
		if (m_components[i]->m_componentName == targetComponent)
		{
			return true;
		}
	}

	return false;
}

void GameObject::RemoveComponent(std::string componentName)
{
	// Loops through each component in the vector.
	for (int i = 0; i < m_components.size(); i++)
	{
		// Checks if the selected component has the same name as the search term.
		if (m_components[i]->m_componentName == componentName)
		{
			m_components.erase(m_components.begin() + i); // Delete the component.
		}
	}
}