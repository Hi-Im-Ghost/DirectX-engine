#pragma once
#include "Window.h"
#include "Timer.h"
#include "Camera.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include <set>

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
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
	ImguiManager imgui;
	Window win;
	Timer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class Box*> boxes;
	Camera cam;
	PointLight light;
	//Liczba obiektów
	static constexpr size_t nDrawables = 180;
	//Zmienna do zarzadzania predkoscia obiektow
	float speed_factor = 1.0f;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};

