#include <iostream>
#include "orbiter.h"
#include "editor.h"

int main()
{
	Application a;
	a.applicationType = ApplicationType::EditorOB;

	Editor editor;
	Core.renderingLayer = &editor;

	a.Run();
	delete Core.renderingLayer;
	return 0;
}