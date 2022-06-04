#pragma once
#include "Window.h"
#include "Timer.h"

#include "ImguiManager.h"

class App
{
public:
	App();
	// Main loop
	int Start();
	~App();
private:
	//Wykonywanie co klatke
	void DoFrame();
private:
	ImguiManager imgui;
	Window win;
	Timer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	//Liczba obiektów
	static constexpr size_t nDrawables = 180;
};

