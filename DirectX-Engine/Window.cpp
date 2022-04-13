#include "Window.h"
#include <sstream>
#include "resource.h"
//
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept:
	hInst(GetModuleHandle(nullptr)) //Uzyskiwanie instancji do zapisywania danych
{
	WNDCLASSEX winclass = { 0 }; //struktura konfiguracyjna 
	winclass.cbSize = sizeof(winclass); //ustawienie rozmiaru struktury 
	winclass.style = CS_OWNDC; //ustawienie stylu
	winclass.lpfnWndProc = HandleMsgSetup; //wskaŸnik do procedury okna
	winclass.cbClsExtra = 0; //iloœæ dodatkowych bajtów w strukturze po stronie API
	winclass.cbWndExtra = 0; //iloœæ dodatkowych bajtów dla ka¿dego okna, które zosta³o stworzone przez t¹ klase
	winclass.hInstance = GetInstance(); //dojœcie do instancji aplikacji
	winclass.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); //ikona 
	winclass.hCursor = nullptr; //kursor
	winclass.hbrBackground = nullptr; //t³o
	winclass.lpszMenuName = nullptr; //nazwa menu
	winclass.lpszClassName = GetName(); //nazwa klasy
	winclass.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); ; //ikona
	RegisterClassEx(&winclass); //rejestracja klasy. Ex oznacza nowsz¹ wersje
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


// 
Window::Window(int width, int height, const char* name) 
{
	// rozmiar okna
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	//Dostosowanie by rozmiar okna by³ dla ca³oœci po¿¹danego regionu klienta
	if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)))
	{
		throw CHWND_LAST_EXCEPT();
	};
	//Instancja okna
	hWnd = CreateWindow(
		WindowClass::GetName(), //nazwa klasy
		name, //nazwa okna
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, //po³o¿enie okna
		wr.bottom - wr.top, //rozdzielczoœæ
		nullptr, //wskaŸnik na rodzica okna
		nullptr, //wskaŸnik do menu
		WindowClass::GetInstance(), //instancja
		this //niestandardowy
	);
	//Sprawdzanie bledow
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	//Wyœwietl okno
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}


LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Sprawdzenie czy typ wiadomosci jest rowny tworzonemu bez uzytkownika
	if (msg == WM_NCCREATE)
	{
		// wyodrêbnij wskaŸnik do klasy okna z danych tworzenia
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		// statyczne rzutowanie tworzenia okna 
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// ustaw dane u¿ytkownika zarz¹dzane przez WinAPI do przechowywania wskaŸnika do klasy okna
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// ustaw procedurê obs³ugi wiadomoœci na normaln¹ (nie-instalacyjn¹) procedurê obs³ugi po zakoñczeniu instalacji
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// przeœlij wiadomoœæ do programu obs³ugi klasy okna
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	//Jesli nie to zrob domyslnie
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// pobierz wskaznik do klasy okna
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// przeœlij wiadomoœæ do programu obs³ugi klasy okna
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

//Procedura dla okna
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	//Akcja po nacisnieciu X okna
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
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

Window::Exceptions::Exceptions(int line, const char* file, HRESULT hr) noexcept:
	Exception(line, file),
	hr(hr)
{}

const char* Window::Exceptions::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "Error Code: " << GetErrorCode() << std::endl
		<< "Description: " << GetErrorString() << std::endl
		<< GetOriginString();
	buffer = oss.str();
	return buffer.c_str();
}

const char* Window::Exceptions::GetType() const noexcept
{
	return "Window Exception";
}

std::string Window::Exceptions::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	//Daj opis dla tego b³êdu
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exceptions::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exceptions::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}