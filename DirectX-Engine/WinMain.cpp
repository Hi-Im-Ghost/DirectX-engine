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
			//Wpisz w miejsce tytu³u okna
			SetWindowText(hWnd, "Nazwa zmieniona :)");
		}
		break;
		//Akcja po puszczeniu klawisza
	case WM_KEYUP:
		if (wParam == 'F')
		{
			//Wpisz w miejsce tytu³u okna
			SetWindowText(hWnd, "Pusciles klawisz :)");
		}
		break;
	//Ciagi znakow 
	case WM_CHAR:
		{
		static std::string temp;
		//Zapisuj znaki do zmiennej 
		temp.push_back((char)wParam);
		//Wpisz w miejsce tytu³u okna
		SetWindowText(hWnd, temp.c_str());
		}
		break;
	//Lewy przycisk myszy
	case WM_LBUTTONDOWN: 
		{
		//Pobierz wspó³rzêdne myszy
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream os;
		os << "<" << pt.x << "," << pt.y << ">";
		//Wpisz w miejsce tytu³u okna
		SetWindowText(hWnd, os.str().c_str());
		}
		break;
	}
	//Zwracamy procedure okna
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain( 
	HINSTANCE hInstance, //wskaŸniki do struktur, które przechowuj¹ informacje o naszym programie
	HINSTANCE hPrevInstance, //w którym s¹ ³adowane do pamiêci. Drugi zawsze bêdzie NULL. 
	LPSTR lpCmdLine, //Linia poleceñ jako pojedynczy ci¹g znaków
	int nCmdShow) //Info w jaki sposób okno powinno byæ pokazane
{
	const auto pClassName = "DirectX-Engine";
	//Klasa okna
	WNDCLASSEX winclass = { 0 }; //struktura konfiguracyjna 
	winclass.cbSize = sizeof(winclass); //ustawienie rozmiaru struktury 
	winclass.style = CS_OWNDC; //ustawienie stylu
	winclass.lpfnWndProc = WndProc; //wskaŸnik do procedury okna
	winclass.cbClsExtra = 0; //iloœæ dodatkowych bajtów w strukturze po stronie API
	winclass.cbWndExtra = 0; //iloœæ dodatkowych bajtów dla ka¿dego okna, które zosta³o stworzone przez t¹ klase
	winclass.hInstance = hInstance; //dojœcie do instancji aplikacji
	winclass.hIcon = nullptr; //ikona kursora
	winclass.hCursor = nullptr; //kursor
	winclass.hbrBackground = nullptr; //t³o
	winclass.lpszMenuName = nullptr; //nazwa menu
	winclass.lpszClassName = pClassName; //nazwa klasy
	winclass.hIconSm = nullptr; //ikona
	RegisterClassEx(&winclass); //rejestracja klasy. Ex oznacza nowsz¹ wersje
	//Instancja okna
	HWND hWnd = CreateWindowEx(
		0, //style
		pClassName, //nazwa klasy
		"DEngine", //nazwa okna
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200, //po³o¿enie okna
		640, 480, //rozdzielczoœæ
		nullptr, //wskaŸnik na rodzica okna
		nullptr, //wskaŸnik do menu
		hInstance, //instancja
		nullptr //niestandardowy
	);
	//Wyœwietl okno
	ShowWindow(hWnd, SW_SHOW);

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