#include "GameObjects/gameobject.h"
#include "gtc/matrix_transform.hpp"

GameObject::GameObject()
{
	objectName = "New GameObject";
}

glm::mat4 GameObject::GetView() const
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(-transform.position.x, -transform.position.y, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(-transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

bool GameObject::HasComponent(const std::string targetComponent)
{
	for (int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->m_componentName == targetComponent)
		{
			return true;
		}
	}

	return false;
}

//void GameObject::AddComponent(Component newComponent)
//{
//	m_components.push_back(std::make_unique<Component>(newComponent));
//}

void GameObject::RemoveComponent(std::string componentName)
{
	for (int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->m_componentName == componentName)
		{
			m_components.erase(m_components.begin() + i);
		}
	}
}