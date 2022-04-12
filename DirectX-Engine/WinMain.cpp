#include <Windows.h>
#include "WindowsMessageManager.h"
#include <sstream>
//Procedura dla okna
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageManager message;
	OutputDebugString(message(msg, lParam, wParam).c_str());
	switch (msg)
	{
	//Akcja po nacisnieciu X okna
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	//Akcja po naciscieniu klawisza
	case WM_KEYDOWN:
		if (wParam == 'F')
		{
			//Wpisz w miejsce tytu�u okna
			SetWindowText(hWnd, "Nazwa zmieniona :)");
		}
		break;
		//Akcja po puszczeniu klawisza
	case WM_KEYUP:
		if (wParam == 'F')
		{
			//Wpisz w miejsce tytu�u okna
			SetWindowText(hWnd, "Pusciles klawisz :)");
		}
		break;
	//Ciagi znakow 
	case WM_CHAR:
		{
		static std::string temp;
		//Zapisuj znaki do zmiennej 
		temp.push_back((char)wParam);
		//Wpisz w miejsce tytu�u okna
		SetWindowText(hWnd, temp.c_str());
		}
		break;
	//Lewy przycisk myszy
	case WM_LBUTTONDOWN: 
		{
		//Pobierz wsp�rz�dne myszy
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream os;
		os << "<" << pt.x << "," << pt.y << ">";
		//Wpisz w miejsce tytu�u okna
		SetWindowText(hWnd, os.str().c_str());
		}
		break;
	}
	//Zwracamy procedure okna
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain( 
	HINSTANCE hInstance, //wska�niki do struktur, kt�re przechowuj� informacje o naszym programie
	HINSTANCE hPrevInstance, //w kt�rym s� �adowane do pami�ci. Drugi zawsze b�dzie NULL. 
	LPSTR lpCmdLine, //Linia polece� jako pojedynczy ci�g znak�w
	int nCmdShow) //Info w jaki spos�b okno powinno by� pokazane
{
	const auto pClassName = "DirectX-Engine";
	//Klasa okna
	WNDCLASSEX winclass = { 0 }; //struktura konfiguracyjna 
	winclass.cbSize = sizeof(winclass); //ustawienie rozmiaru struktury 
	winclass.style = CS_OWNDC; //ustawienie stylu
	winclass.lpfnWndProc = WndProc; //wska�nik do procedury okna
	winclass.cbClsExtra = 0; //ilo�� dodatkowych bajt�w w strukturze po stronie API
	winclass.cbWndExtra = 0; //ilo�� dodatkowych bajt�w dla ka�dego okna, kt�re zosta�o stworzone przez t� klase
	winclass.hInstance = hInstance; //doj�cie do instancji aplikacji
	winclass.hIcon = nullptr; //ikona kursora
	winclass.hCursor = nullptr; //kursor
	winclass.hbrBackground = nullptr; //t�o
	winclass.lpszMenuName = nullptr; //nazwa menu
	winclass.lpszClassName = pClassName; //nazwa klasy
	winclass.hIconSm = nullptr; //ikona
	RegisterClassEx(&winclass); //rejestracja klasy. Ex oznacza nowsz� wersje
	//Instancja okna
	HWND hWnd = CreateWindowEx(
		0, //style
		pClassName, //nazwa klasy
		"DEngine", //nazwa okna
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, //po�o�enie okna
		640, 480, //rozdzielczo��
		nullptr, //wska�nik na rodzica okna
		nullptr, //wska�nik do menu
		hInstance, //instancja
		nullptr //niestandardowy
	);
	//Wy�wietl okno
	ShowWindow(hWnd, SW_SHOW);

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