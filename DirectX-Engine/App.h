#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
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
	Camera cam;
	//Liczba obiektów
	static constexpr size_t nDrawables = 180;
	//Zmienna do zarzadzania predkoscia obiektow
	float speed_factor = 1.0f;
};

