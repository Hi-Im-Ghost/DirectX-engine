#include "App.h"
#include <sstream>

int CALLBACK WinMain( 
	HINSTANCE hInstance, //wskaźniki do struktur, które przechowują informacje o naszym programie
	HINSTANCE hPrevInstance, //w którym są ładowane do pamięci. Drugi zawsze będzie NULL. 
	LPSTR lpCmdLine, //Linia poleceń jako pojedynczy ciąg znaków
	int nCmdShow) //Info w jaki sposób okno powinno być pokazane
{
	try {
		return App{}.Start();
	}
	catch (const Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "WinMain Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}