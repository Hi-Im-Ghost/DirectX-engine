#include "Window.h"


int CALLBACK WinMain( 
	HINSTANCE hInstance, //wskaŸniki do struktur, które przechowuj¹ informacje o naszym programie
	HINSTANCE hPrevInstance, //w którym s¹ ³adowane do pamiêci. Drugi zawsze bêdzie NULL. 
	LPSTR lpCmdLine, //Linia poleceñ jako pojedynczy ci¹g znaków
	int nCmdShow) //Info w jaki sposób okno powinno byæ pokazane
{
	Window wind(800, 480, "DenginX");

	//Struktura wiadomoœci
	MSG msg;
	//
	BOOL gResult;
	//Pêtla g³ówna
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		//Przetwarzanie wiadomosci
		TranslateMessage(&msg);
		//Przekazanie wiadomoœci do okna
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