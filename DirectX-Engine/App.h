#pragma once
#include "Window.h"
class App
{
public:
	App();
	// Main loop
	int Start();
private:
	void DoFrame();
private:
	Window win;
};

