#include <iostream>
#include "orbiter.h"
#include "editor.h"

int main()
{
	Application a;

	Editor editor;
	Core.renderingLayer = &editor;

	a.Run();
	delete Core.renderingLayer;
	return 0;
}