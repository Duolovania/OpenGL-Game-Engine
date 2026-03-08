#include "Scripts/script.h"

Script::Script()
{

}

void Script::SetPath(std::string filePath)
{
	m_path = filePath;
}

std::string Script::GetPath() const
{
	return m_path;
}
