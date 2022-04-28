#include "App.h"

App::App()
	:
	win(800, 600, "DenginX")
{}

int App::Start()
{
	while (true)
	{
		// Przetwarzanie powiadomien
		if (const auto code = Window::ProcessMessages())
		{
			// Jeœli wychodzimy to zwracamy kod wyjœcia
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
	win.D3g().DrawTriangle();
	win.D3g().EndFrame();
}