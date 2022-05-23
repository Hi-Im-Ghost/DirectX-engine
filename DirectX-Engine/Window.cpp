#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowsThrowMacros.h"

//
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept:
	hInst(GetModuleHandle(nullptr)) //Uzyskiwanie instancji do zapisywania danych
{
	WNDCLASSEX winclass = { 0 }; //struktura konfiguracyjna 
	winclass.cbSize = sizeof(winclass); //ustawienie rozmiaru struktury 
	winclass.style = CS_OWNDC; //ustawienie stylu
	winclass.lpfnWndProc = HandleMsgSetup; //wskaźnik do procedury okna
	winclass.cbClsExtra = 0; //ilość dodatkowych bajtów w strukturze po stronie API
	winclass.cbWndExtra = 0; //ilość dodatkowych bajtów dla każdego okna, które zostało stworzone przez tą klase
	winclass.hInstance = GetInstance(); //dojście do instancji aplikacji
	winclass.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); //ikona 
	winclass.hCursor = nullptr; //kursor
	winclass.hbrBackground = nullptr; //tło
	winclass.lpszMenuName = nullptr; //nazwa menu
	winclass.lpszClassName = GetName(); //nazwa klasy
	winclass.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); ; //ikona
	RegisterClassEx(&winclass); //rejestracja klasy. Ex oznacza nowszą wersje
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
Window::Window(int width, int height, const char* name):
	width(width),
	height(height)
{
	// rozmiar okna
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	//Dostosowanie by rozmiar okna był dla całości pożądanego regionu klienta
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)==0)
	{
		throw CHWND_LAST_EXCEPT();
	}
	//Instancja okna
	hWnd = CreateWindow(
		WindowClass::GetName(), //nazwa klasy
		name, //nazwa okna
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, //położenie okna
		wr.bottom - wr.top, //rozdzielczość
		nullptr, //wskaźnik na rodzica okna
		nullptr, //wskaźnik do menu
		WindowClass::GetInstance(), //instancja
		this //niestandardowy
	);
	//Sprawdzanie bledow
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	//Wyświetl okno
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	//Tworzenie obrazu grafiki d3d
	gD3g = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{

	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}


std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	// Gdy kolejka zawiera powiadomienia to je usuń i wyślij
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Sprawdzanie czy wyszlismy 
		if (msg.message == WM_QUIT)
		{
			// opcjonalnie zwróc sygnalizacje do wyjscia
			return (int)msg.wParam;
		}

		// Publikowanie powiadomien
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Nic nie zwraca gdy nie zamykamy aplikacji
	return {};
}

Graphics& Window::D3g()
{
	//Wyjątek gdy spróbujemy uzyskać grafikę ale wskaźnik nie został jeszcze ustawiony
	if (!gD3g)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *gD3g;
}



LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Sprawdzenie czy typ wiadomosci jest rowny tworzonemu bez uzytkownika
	if (msg == WM_NCCREATE)
	{
		// wyodrębnij wskaźnik do klasy okna z danych tworzenia
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		// statyczne rzutowanie tworzenia okna 
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// ustaw dane użytkownika zarządzane przez WinAPI do przechowywania wskaźnika do klasy okna
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// ustaw procedurę obsługi wiadomości na normalną (nie-instalacyjną) procedurę obsługi po zakończeniu instalacji
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// prześlij wiadomość do programu obsługi klasy okna
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	//Jesli nie to zrob domyslnie
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// pobierz wskaznik do klasy okna
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// prześlij wiadomość do programu obsługi klasy okna
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//Procedura dla okna
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Przekierowanie inputu do ImGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	//Akcja po nacisnieciu X okna
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	//wyczyść stan klawiszy, gdy okno traci fokus, aby zapobiec "zablokowaniu" danych wejściowych
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	//KLAWIATURA
	//Akcja po naciscieniu klawisza
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) //autorepeat
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	//MYSZKA
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// w oknie przenos i przechwycaj
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// jesli nie w oknie to utrzymuj przechwytwanie jesli trzymamy przycisk
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// jesli przycisk puscimy to zwolnij przechtytywanie 
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	}

	//Zwracamy procedure okna
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Wyjątki okna
std::string Window::Exceptions::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// Przydziel pamięc dla łańcucha błędów i wskaż na niego nasz wskaźnik
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// Zwrócona długość ciągu 0 oznacza awarię
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// skopiuj ciąg błędu z bufora przydzielonego do systemu Windows do std::string
	std::string errorString = pMsgBuf;
	// zwolnij bufor systemu
	LocalFree(pMsgBuf);
	return errorString;
}


Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	buffer = oss.str();
	return buffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exceptions::TranslateErrorCode(hr);
}


const char* Window::NoGfxException::GetType() const noexcept
{
	return "Window Exception [No Graphics]";
}