#include "App.h"

App::App() : win(800, 600, "DenginX")
{}

int App::Start()
{
	while (true)
	{
		// Przetwarzanie powiadomien
		if (const auto code = Window::ProcessMessages())
		{
			// Jeśli wychodzimy to zwracamy kod wyjścia
			return *code;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	//Przy pomocy sin w czasie manipulujemy kolorem
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	win.D3g().ClearBuffer(c, 1.0f, c);
	win.D3g().DrawCube(
		3.0f,
		3.0f,
		6.0f,

		0.0f,
		timer.Peek(),
		timer.Peek()
	);
	win.D3g().DrawCube(
		win.mouse.GetPosX() / 400.0f - 1.0f,
		-win.mouse.GetPosY() / 300.0f + 1.0f,
		4.0f,

		45.0f,
		45.0f,
		0.0f
	);
	win.D3g().DoFrame();
	win.D3g().EndFrame();
}