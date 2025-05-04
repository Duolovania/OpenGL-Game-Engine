#include <iostream>
#include "orbiter.h"
#include "launcher.h"

int main()
{
	Application a;
	a.applicationType = ApplicationType::LauncherOB;
	a.SetScreenResolution(854, 640);

	Launcher launcher;
	Core.renderingLayer = &launcher;

	a.Run();
	delete Core.renderingLayer;
	return 0;
}