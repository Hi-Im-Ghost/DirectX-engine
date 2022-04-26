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

}