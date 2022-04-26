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
			// Je�li wychodzimy to zwracamy kod wyj�cia
			return *code;
		}
		DoFrame();
	}
}

void App::DoFrame()
{

}