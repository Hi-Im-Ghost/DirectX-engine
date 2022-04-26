#include "App.h"
#include <sstream>

int CALLBACK WinMain( 
	HINSTANCE hInstance, //wskaŸniki do struktur, które przechowuj¹ informacje o naszym programie
	HINSTANCE hPrevInstance, //w którym s¹ ³adowane do pamiêci. Drugi zawsze bêdzie NULL. 
	LPSTR lpCmdLine, //Linia poleceñ jako pojedynczy ci¹g znaków
	int nCmdShow) //Info w jaki sposób okno powinno byæ pokazane
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