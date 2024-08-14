#include <iostream>
#include "Headers/orbiter.h"
#include "Headers/editor.h"

int main()
{
	Application a;

	Editor editor;
	Core.renderingLayer = &editor;

	a.Run();
	delete Core.renderingLayer;
	return 0;
}