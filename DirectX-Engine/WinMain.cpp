#include "App.h"
#include <sstream>

int CALLBACK WinMain( 
	HINSTANCE hInstance, //wska�niki do struktur, kt�re przechowuj� informacje o naszym programie
	HINSTANCE hPrevInstance, //w kt�rym s� �adowane do pami�ci. Drugi zawsze b�dzie NULL. 
	LPSTR lpCmdLine, //Linia polece� jako pojedynczy ci�g znak�w
	int nCmdShow) //Info w jaki spos�b okno powinno by� pokazane
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