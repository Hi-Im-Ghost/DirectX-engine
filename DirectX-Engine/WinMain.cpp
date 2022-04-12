#include "Window.h"


int CALLBACK WinMain( 
	HINSTANCE hInstance, //wska�niki do struktur, kt�re przechowuj� informacje o naszym programie
	HINSTANCE hPrevInstance, //w kt�rym s� �adowane do pami�ci. Drugi zawsze b�dzie NULL. 
	LPSTR lpCmdLine, //Linia polece� jako pojedynczy ci�g znak�w
	int nCmdShow) //Info w jaki spos�b okno powinno by� pokazane
{
	Window wind(800, 480, "DenginX");

	//Struktura wiadomo�ci
	MSG msg;
	//
	BOOL gResult;
	//P�tla g��wna
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		//Przetwarzanie wiadomosci
		TranslateMessage(&msg);
		//Przekazanie wiadomo�ci do okna
		DispatchMessage(&msg);
	}
	if (gResult == -1)
	{
		return -1;
	}
	else
	{
		return msg.wParam;
	}

}