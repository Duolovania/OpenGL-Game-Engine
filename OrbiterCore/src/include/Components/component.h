#pragma once
#include <string>

class Component
{
	public:
		virtual ~Component() = default;
		std::string m_componentName;
};